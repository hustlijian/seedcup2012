/**
 * @file	CmdProcess.c
 * @author  lijian <hustlijian@gmail.com>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * 代码的解析部分，解析命令，调用对应的底层API
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "CmdProcess.h"
#include "expression.h"
#include "DatabaseAPI.h"
#include "Tree.h"

#define COL_NUM		20
#define NAME_LENGHT 25

char keywords[NUM_KEYWORDS][MAX_KEYWORDS_LENGTH]={
	"create", "database","table", "alter","truncate","add","column","use",
		"drop","rename","select","from","where","order","by", "desc", "incr", 
		"update","set","delete", "insert", "into", "values", "show", 
		"databases","int","float","text", "none","between","like","and", "or"
};
char operatorwords[NUM_OPERATOR][MAX_OPERATOR] = { 
	"(", ")","*", "/", "%","+", "-", "==", "~=", ">=", "<=", ">", "<","[",
		"]",";",",","'","?","="
};

char *p;  //当前处理到的字符位置
char word[MAX_WORDLEGTH]; //保存当前的获得的单词
int syn;  //保存当前的类型

/*功能：
 *     不区分大小写的字符比较
 * 参数：
 * 
*/
int mystrcmp(const char *str1, const char *str2)
{
	int i=0;
	char c1, c2;
	do 
	{
		c1=tolower(str1[i]);
		c2=tolower(str2[i++]);
	} while ((c1==c2)&&c1&&c2);
	return (c1-c2);
}
//释放value中的textvalue
void freeValue(Value *value)
{
	if (value->columnType == TEXT)
		free(value->columnValue.textValue);
}
/* 功能：
 *		获得string的类型
 * 参数：
 * @str:  没有空格的字符串
 * 返回：
 *		整形，类型码,其中0代表不正确的输入字符
 */
int getTypeNum(char *str)
{
	int i, flag;
	
	for (i=0, flag=0;str[i]&&(isdigit(str[i])||str[i]=='.');i++)
		if (str[i]=='.'){
			flag++;
		}
	if (str[i]=='\0')//数字
	{
		if (flag==1) return SYN_FLOAT_NUMBER; //浮点数
		if (flag>1) return 0;//错误输入
		return SYN_INTEGER_NUMBER; //整数
	} 
	if (isalpha(str[0])||str[0]=='_') //标识符
	{
		for (i=0;i<NUM_KEYWORDS;i++)
			if (!mystrcmp(keywords[i], str))
				break;
		if (i==NUM_KEYWORDS) return SYN_IDENTIFIER;//一般标识符
		else return (i+1);
	}
	//运算符
	for (i=0;i<NUM_OPERATOR;i++)
		if (!strcmp(str, operatorwords[i]))
			break;
	if (i<NUM_OPERATOR) return (i+SYN_OPERATOR_BASE);
	else return 0;
	return 0;
}
/*
 *功能：
 *		获取值类型
 */
COLUMN_TYPE getValueType(char *str)
{
	switch (getTypeNum(str))
	{
	case SYN_INTEGER_NUMBER:
		return INT;
	case SYN_FLOAT_NUMBER:
		return FLOAT;
	case SYN_IDENTIFIER:
		return TEXT;
	default:	//这句是为了取消warning，其实不需要
		return EMPTY;
	}
}
/*
 *功能：
 *		获取操作符类型
 */
OPERATOR getOptType(char *str)
{
	switch (getTypeNum(str))
	{
	case SYN_EQUAL:
		return EQ;
	case SYN_NOT_EQUAL:
		return NE;
	case SYN_MORE_EQUAL:
		return GET;
	case SYN_LESS_EQUAL:
		return LET;
	case SYN_GREATER:
		return GT;
	case SYN_LESS:
		return LT;
	case SYN_BETWEEN:
		return BETWEEN;
	case SYN_LIKE:
		return LIKE;
	default:	//这句是为了取消warning，其实不需要
		return EQ;
	}
}
/*
 *功能：
 *		判断是否是操作符
 */
int isLgcExpOpt(char *str)
{
	switch (getTypeNum(str))
	{
	case SYN_EQUAL:
	case SYN_NOT_EQUAL:
	case SYN_MORE_EQUAL:
	case SYN_LESS_EQUAL:
	case SYN_GREATER:
	case SYN_LESS:
	case SYN_BETWEEN:
	case SYN_LIKE:
		return 0;
	default:	
		return -1;
	}
}
/*
 *功能：
 *		获取逻辑类型
 */
LOGIC getLogicType(char *str)
{
	switch (getTypeNum(str))
	{
	case SYN_AND:
		return AND;
	case SYN_OR:
		return OR;
	default:
		return NOLOGIC;
	}
}
/*
 *功能:
 *     扫描字符串p，保存一个词到word
 */
 void scaner()
{
	int i, j;
	char ch, temp;

	for (i=0; i<MAX_WORDLEGTH; i++)
		word[i] = '\0';

	i = 0;
	if(p==NULL) {
		syn = SYN_ELSE;
		word[0]='\0';
		return;
	}
	while((isspace(ch=p[i++]))) ; /* 越过空字符*/
	if (ch&&(isalnum(ch)||ch=='_'||ch=='.'))  /*单词或数字*/
	{
		j = 0;
		while(ch&&(isalnum(ch)||ch=='_'||ch=='.')) {
			word[j++] = ch;
			ch = p[i++];
		}
		word[j] = '\0';
		syn = getTypeNum(word); 
	}
	else if(ch){   //运算符等
		j = 0;
		word[j++] = ch;
		temp = ch;
		ch = p[i++];
		switch (temp) {
			case '=':
				if (ch!='=')
					break;
				//fall through
			case '~': 
				if (ch!='=')
					break;
				//fall through
			case '<':
				if (ch != '=')
					break;
				//fall through
			case '>':
				if (ch != '=') 
					break;

				word[j++] = ch;				
				i++;				
				break;
			default:
				break;
		}
		word[j] = '\0';
		syn = getTypeNum(word); 
	} else //空
	{
		syn = SYN_ELSE;
		word[0]='\0';
	}
	p = p+i-1;
	if (*p=='\0')
		p=NULL;
}
/*
 *功能：
 *		判断是否是关键字
 */
int isKeywords(char *str)
{
	int i;
	for (i=0;i<NUM_KEYWORDS;i++)
		if (!mystrcmp(str, keywords[i]))
			return 1;
	return 0;  //不是关键字
}
/*
 *功能：
 *		判断是否是符合条件的数据库名，表名，列名
 */
int checkName(char *str)
{
	int i;
	for (i=0;str[i];i++)
		if(!isalpha(str[i])&&str[i]!='_')	//isalpha判断字符是否为英文字母
			return 0;
	return 1;
}
/*
 *功能：
 *		确认到达末尾
 */
int checkEnd()
{
	scaner();
	if (syn==SYN_SEMICOLON || word[0]=='\0')
		return 1;
	return 0;
}
/*
 *功能：
 *		建立数据库命令
 */
int createDatabaseCmd()
{
	char databaseName[NAME_LENGHT];

	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word)) //检查关键字
		return -1;
	if (!checkName(word)) //检查数据名，英文字符，下划线
		return -1;
	strcpy(databaseName, word);

	if (!checkEnd())//确认命令结束
		return -1;
	if (createDatabase(databaseName))//建立数据库
		return -1;
	return 0;
}
/*
 *功能：
 *		建立表命令
 */
int createTableCmd()
{
	int countAmount, i;
	char columnsName[COL_NUM][NAME_LENGHT];
	char tableName[NAME_LENGHT];
	COLUMN_TYPE columnType[COL_NUM];
	char *p[COL_NUM];

	scaner();
	if (syn!=SYN_IDENTIFIER || isKeywords(word))//表名使用关键字
		return -1;
	if (!checkName(word)) //检查数据名，英文字符，下划线
		return -1;
	strcpy(tableName,word);


	if (checkEnd())//无列表
	{
		if(createTable(tableName,NULL, NULL, 0))
			return -1;
		return 0;
	}
	if(syn!=SYN_PAREN_LEFT)	//'('
		return -1;

	scaner();
	if (syn == SYN_PAREN_RIGHT)//')'
	{
		if(createTable(tableName,NULL, NULL, 0))
			return -1;
		if (!checkEnd())
			return -1;
		return 0;
	}
	for (countAmount=0;countAmount<COL_NUM;countAmount++,scaner())
	{
		
		if(syn!=SYN_IDENTIFIER||isKeywords(word)) 
			return -1;
		if (!checkName(word)) //检查数据名，英文字符，下划线
			return -1;
		strcpy(columnsName[countAmount], word);

		scaner();
		if(syn>=SYN_INT&&syn<=SYN_NONE)  //指定类型int26,float27,text28
			columnType[countAmount] = syn-SYN_INT;
		else {
			columnType[countAmount] = NONE;
			if (syn==SYN_PAREN_RIGHT) break; //')'
			if (syn==SYN_COMMA) continue; //逗号
			else return -1;
		}

		scaner();
		if (syn==SYN_PAREN_RIGHT) break; //右括号
		if (syn!=SYN_COMMA) return -1; //逗号
	}

	for (i=0;i<=countAmount;i++)
	{
		p[i]=columnsName[i];
	}
	
	if (!checkEnd())//确认命令结束
		return -1;
	if(createTable(tableName, p, columnType, countAmount+1))
		return -1;
	return 0;
}
/*
 *功能：
 *		建立数据库或者表
 */
int createCmd()
{
	scaner();
	if(syn==SYN_DATABASE)	//database
	{
		return createDatabaseCmd();
	} else if (syn==SYN_TABLE)	//table
	{
		return createTableCmd();
	} else 
		return -1;
}
/*
 *功能：
 *		添加列
 */
int alterAddCmd(char *tableName)
{
	char columnName[NAME_LENGHT];
	COLUMN_TYPE columnType;

	scaner();
	if (syn != SYN_IDENTIFIER)
		return -1;
	if (!checkName(word)) //名，英文字符，下划线
		return -1;
	strcpy(columnName,word);

	scaner();
	if(syn>=SYN_INT&&syn<=SYN_TEXT)  //指定类型26,27,28
		columnType = syn-SYN_INT;
	else if (syn==SYN_SEMICOLON) 
		columnType = NONE;
	else
		return -1;

	if (!checkEnd())//确认命令结束
		return -1;

	if (addColumn(tableName,columnName,columnType))
		return -1;
	return 0;		
}
/*
 *功能：
 *		删除列
 */
int alterRmCmd(char *tableName)
{
	char columnName[NAME_LENGHT];
	scaner();
	if (syn!=SYN_COLUMN)
		return -1;

	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;
	if (!checkName(word)) //名，英文字符，下划线
		return -1;
	strcpy(columnName,word);

	if (!checkEnd())//确认命令结束
		return -1;
	if (rmColumn(tableName,columnName))
		return -1;
	return 0;
}
/*
 *功能：
 *		修改列类型
 */
int alterAlterCmd(char *tableName)
{
	char columnName[NAME_LENGHT];
	COLUMN_TYPE columnType;
	
	scaner();
	if (syn!=SYN_COLUMN)
		return -1;

	scaner();
	if (syn!=SYN_IDENTIFIER)
		return -1;
	if (!checkName(word)) //名，英文字符，下划线
		return -1;
	strcpy(columnName,word);
	
	scaner();
	if(syn>=SYN_INT&&syn<=SYN_TEXT)  //指定类型26,27,28
		columnType = syn-SYN_INT;
	else if (syn==SYN_SEMICOLON) 
		columnType = NONE;
	else
		return -1;

	if (!checkEnd())//确认命令结束
		return -1;
	if (alterColumn(tableName,columnName,columnType))
		return -1;

	return 0;	

}
/*
 *功能：
 *		修改列
 */
int alterCmd()
{
	char tableName[NAME_LENGHT];
	scaner();
	if (syn!=SYN_TABLE)
		return -1;

	scaner();
	if (syn!=SYN_IDENTIFIER)
		return -1;
	if (!checkName(word)) //名，英文字符，下划线
		return -1;
	strcpy(tableName,word);

	scaner();
	if (syn==SYN_ADD)//add
	{
		return alterAddCmd(tableName);
	} else if (syn==SYN_DROP)//drop
	{
		return alterRmCmd(tableName);
	}else if (syn ==SYN_ALTER)//alter
	{
		return alterAlterCmd(tableName);
	}else 
		return -1;
}
/*
 *功能：
 *		清除表数据
 */
int truncateCmd()
{
	char tableName[NAME_LENGHT];
	scaner();
	if (syn!=SYN_TABLE)
		return -1;
	
	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;
	if (!checkName(word)) //名，英文字符，下划线
		return -1;
	strcpy(tableName, word);

	if (!checkEnd())//确认命令结束
		return -1;
	if (truncateTable(tableName))
		return -1;
	return 0;
}
/*
 *功能：
 *		指定数据库
 */
int useCmd()
{
	char databaseName[NAME_LENGHT];
	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;
	if (!checkName(word)) //名，英文字符，下划线
		return -1;
	strcpy(databaseName, word);

	if (!checkEnd())//确认命令结束
		return -1;
	if (use(databaseName))
		return -1;
	return 0;
}
/*
 *功能：
 *		删除表或数据库
 */
int dropCmd()
{
	char databaseName[NAME_LENGHT];
	char tableName[NAME_LENGHT];

	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;
	if (!checkName(word)) //名，英文字符，下划线
		return -1;
	strcpy(databaseName, word);

	scaner();
	if (syn==SYN_SEMICOLON)
		return drop(databaseName, NULL);
	if (syn==SYN_IDENTIFIER){
		if (!checkName(word)) //名，英文字符，下划线
			return -1;
		strcpy(tableName, word);
	}
	else
		return -1;

	if (!checkEnd())//确认命令结束
		return -1;
	if(drop(databaseName,tableName)) 
			return -1;
	return 0;
}
/*
 *功能：
 *		重命名数据库
 */
int renameDatabaseCmd()
{
	char oldName[NAME_LENGHT];
	char newName[NAME_LENGHT];

	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;
	if (!checkName(word)) //名，英文字符，下划线
		return -1;
	strcpy(oldName, word);

	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;
	if (!checkName(word)) //名，英文字符，下划线
		return -1;
	strcpy(newName, word);
	
	if (!checkEnd())//确认命令结束
		return -1;
	if (renameDatabase(oldName, newName))
		return -1;
	return 0;
}
/*
 *功能：
 *		重命名表
 */
int renameTableCmd()
{
	char oldName[NAME_LENGHT];
	char newName[NAME_LENGHT];
	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;
	if (!checkName(word)) //名，英文字符，下划线
		return -1;
	strcpy(oldName, word);
	
	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;
	if (!checkName(word)) //名，英文字符，下划线
		return -1;
	strcpy(newName, word);
	
	if (!checkEnd())//确认命令结束
		return -1;
	if (renameTable(oldName, newName))
		return -1;
	return 0;
}
/*
 *功能：
 *		处理重命名
 */
int renameCmd()
{
	scaner();
	if (syn==SYN_DATABASE)//数据库
	{
		return renameDatabaseCmd();
	} else if(syn==SYN_TABLE)//表
	{
		return renameTableCmd();
	}
	else return -1;
}
/*
 *功能：
 *		确认排序的类型
 */
int  checkSort(SORT_ORDER *sortOrder)
{
	scaner();
	if (syn==SYN_SEMICOLON) //NOSORT
		*sortOrder = NOTSORT;
	else if (syn==SYN_ORDER)//ORDER
	{
		scaner();
		if (syn!=SYN_BY)//by
			return -1;
		scaner();
		if (syn==SYN_DESC)
			*sortOrder = DESC;
		else if (syn==SYN_INCR)
			*sortOrder = INCR;
		else 
			return -1;
	} else
		return -1;
	return 0;
}
/*
 *功能：
 *		显示数据库
 */
int showDatabaseCmd()
{
	SORT_ORDER sortOrder;

	if (checkSort(&sortOrder))
		return -1;		
	
	if (!checkEnd())//确认命令结束
		return -1;
	if(showDatabase(sortOrder))
		return -1;
	return 0;
}
/*
 *功能：
 *		显示数据表格
 */
int showTableCmd(char *databaseName)
{//showTable(char *databaseName,SORT_ORDER sortOrder);  //databaseName为NULL时指定为当前数据库
	SORT_ORDER sortOrder;

	if (checkSort(&sortOrder))
		return -1;

	if (!checkEnd())//确认命令结束
		return -1;
	if(showTable(databaseName, sortOrder))
		return -1;
	return 0;
}
/*
 *功能：
 *		显示列名
 */
int showColumnCmd(char *tableName)
{//showColumn(char *tableName, SORT_ORDER sortOrder);
	SORT_ORDER sortOrder;
	
	if (checkSort(&sortOrder))
		return -1;
	
	if (!checkEnd())//确认命令结束
		return -1;
	if(showColumn(tableName, sortOrder))
		return -1;
	return 0;
}
/*
 *功能：
 *		处理显示
 */
int showCmd()
{
	char databaseName[NAME_LENGHT];

	scaner();
	if (syn==SYN_DATABASES)//databases
		return showDatabaseCmd();
	else if(syn==SYN_TABLE)//table
		return showTableCmd(NULL);
	else if(syn==SYN_IDENTIFIER)//database_name,table_name
	{
		if (!checkName(word)) //名，英文字符，下划线
			return -1;
		strcpy(databaseName, word);
	}
	else
		return -1;

	scaner();
	if (syn==SYN_TABLE)//table
		return showTableCmd(databaseName);
	else if (syn==SYN_COLUMN)//column
		return showColumnCmd(databaseName);	
	else 
		return -1;
}
/*
 *功能：
 *		为value赋值
 */
int getValue(Value *value)
{
	char *str;
	int i,num;
	char innerSelect[1024];
	char *temp = p;//保存当前命令位置

	scaner();
	if (syn==SYN_QUOTE)//',类型是字符串
	{
		value->columnType = TEXT;
		str = (char *)malloc(sizeof(char)*NAME_LENGHT);
		if (str==NULL)
			return -1;
		i=0; 
		do {
			str[i]=p[i];
			if(!isalnum(str[i]) && str[i]!=' ' && str[i] != '_' && str[i]!='\'')
			{	//必须是字符数字，或者空格，下划线,单引号 
				return -1;
			}
			i++;
		}while(p[i-1]&&str[i-1]!='\'');
		if (p[i-1] == '\0')
			return -1;
		str[i-1] = '\0';
		p = p + i;

		value->columnValue.textValue = str;
		return 0;
	} else if (syn==SYN_INTEGER_NUMBER)//整数
	{
		value->columnType = INT;
		value->columnValue.intValue = atoi(word);
		return 0;
	}else if (syn==SYN_FLOAT_NUMBER)//浮点数
	{
		value->columnType = FLOAT;
		value->columnValue.floatValue = (float)atof(word);
		return 0;
	} else if (syn==SYN_COMMA)//,.空类型
	{
		value->columnType = EMPTY;
		return 0;
	} else if (syn==SYN_BRACKET_LEFT)//[,like中的text类型
	{
		value->columnType = TEXT;
		str = (char *)malloc(sizeof(char)*NAME_LENGHT);
		if (str==NULL)
			return -1;
		i=0; 
		do {
			str[i]=p[i];
			if(!isalnum(str[i]) && str[i]!=' ' && str[i] != '_' && 
				str[i]!=']' &&str[i] != '*' && str[i] != '?')
			{	//必须是字符数字，或者空格，下划线,单引号 ,问号，星号
				return -1;
			}
			i++;
		}while(p[i-1]&&str[i-1]!=']');
		if (p[i-1] == '\0')
			return -1;
		str[i-1] = '\0';
		p = p + i;
		
		value->columnValue.textValue = str;
		return 0;
	} else if (syn == SYN_PAREN_LEFT || syn == SYN_SELECT)//(
	{
		if (syn == SYN_PAREN_LEFT)
			scaner();
		
		if (syn==SYN_SELECT)//select
		{		
			if (selectCmd(1, value))//内部查询
				return -1;
		} else if (syn==SYN_INTEGER_NUMBER || syn== SYN_FLOAT_NUMBER || 
			syn == SYN_PAREN_LEFT || syn == SYN_MIMUS || syn== SYN_ADD)//int,float,(,-,+
		{
			num = 0; //(的数目
			i=0;
			p=temp;
			while(isspace(*p)) p++;
			do{
				innerSelect[i]=p[i];
				if (p[i]=='(')
					num++;
				if (p[i]==')')
					num--;
				if (p[i]==';' || p[i]=='\0')//end
					return -1;
				i++;
			}while(num>0);
			innerSelect[i]='\0';
			p = p+i;

			if (calExpression(innerSelect, &(value->columnValue.floatValue)))
				return -1;			
			value->columnType = FLOAT;			
		}
		return 0;
	} else
		return -1;
	return 0;
} 
/*
 *功能：
 *		删除命令
 */
int deleteCmd()
{//delete(char *tableName, char *column, Value value);
	char tableName[NAME_LENGHT];
	char colum[NAME_LENGHT];
	Value value;

	scaner();
	if (syn!=SYN_FROM)//from
		return -1;

	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;
	if (!checkName(word)) //名，英文字符，下划线
		return -1;
	strcpy(tableName, word);

	scaner();
	if (syn!=SYN_WHERE)//where
		return -1;

	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;
	if (!checkName(word)) //名，英文字符，下划线
		return -1;
	strcpy(colum, word);

	scaner();
	if (syn!=SYN_EQUAL)//==
		return -1;

	if (getValue(&value))
		return -1;

	if (!checkEnd())//确认命令结束
		return -1;
	if (delete(tableName, colum, value))
		return -1;
	freeValue(&value);

	return 0;
}
/*
 *功能：
 *		处理update命令
 */
int updateCmd()
{//int update(UpdateBody *updateBOdy);
	UpdateBody updateBody;
	char tableName[NAME_LENGHT];
	char columnsName[COL_NUM][NAME_LENGHT];
	char *columns[COL_NUM];
	Value newValues[COL_NUM];
	int columnAmount;
	char updatedColumn[NAME_LENGHT];
	Value oldValue;
	COLUMN_TYPE columnType;
	int i, flag = 0;

	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;
	if (!checkName(word)) //名，英文字符，下划线
		return -1;
	strcpy(tableName, word);

	scaner();
	if (syn!=SYN_SET)//set
		return -1;

	scaner();
	if (syn==SYN_IDENTIFIER)//一列，没有括号
	{
		flag = 1; //没有括号
		if (!checkName(word)) //名，英文字符，下划线
			return -1;
		strcpy(columnsName[0], word);
		columnAmount = 0;
	}else 
	{	
    if (syn!=SYN_PAREN_LEFT)//(
      return -1;
    
    scaner();
    if (syn!=SYN_IDENTIFIER) //columnsNames
      return -1;
	if (!checkName(word)) //名，英文字符，下划线
		return -1;
    strcpy(columnsName[0], word);
    for (scaner(),columnAmount = 0;syn==SYN_COMMA;scaner()) //一直到:)
    {		
      scaner();
      if (syn!=SYN_IDENTIFIER||isKeywords(word))
        return -1;
	  if (!checkName(word)) //名，英文字符，下划线
		return -1;
      strcpy(columnsName[++columnAmount],word);
    }
    if (syn!=SYN_PAREN_RIGHT)//)
      return -1;
  }
	scaner();
	if (syn!=SYN_ASSIGN)//=
		return -1;
		
  if (columnAmount==0)
	{
	  if (flag) //没有括号
	  {
		if (getValue(&newValues[0]))//一列，没有括号
			return -1;
	  } else {
		  scaner();
		  if (syn != SYN_PAREN_LEFT)//(
			return -1;
		  if (getValue(&newValues[0]))//一列，没有括号
			return -1;
		  scaner();
		  if (syn != SYN_PAREN_RIGHT)//)
			return -1;		
	  }
	} else
	{
		scaner();
		if (syn!=SYN_PAREN_LEFT)//(
		  return -1;
		if (getValue(&newValues[0]))
		  return -1;
		for (i=0,scaner();syn==SYN_COMMA;scaner())
		{
		  if (getValue(&newValues[++i]))
			return -1;
		}
		if (i!=columnAmount)//columns == values
		  return -1;
		if (syn!=SYN_PAREN_RIGHT)//)
		  return -1;
  }
	scaner();
	if (syn!=SYN_WHERE)//where
		return -1;

	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;
	if (!checkName(word)) //名，英文字符，下划线
		return -1;
	strcpy(updatedColumn, word);

	scaner();
	if (syn!=SYN_EQUAL)//==
		return -1;
	if (getValue(&oldValue))
		return -1;
	
	if (!checkEnd())//确认命令结束
		return -1;

	for (i=0; i<=columnAmount; i++)
		columns[i] = columnsName[i];
	
	updateBody.tableName = tableName;
	updateBody.columnsName = columns;
	updateBody.newValues = newValues;
	updateBody.columnAmount = columnAmount+1;
	updateBody.updatedColumn = updatedColumn;
	columnType=updateBody.oldValue.columnType = oldValue.columnType;
	switch(columnType)
	{
	case INT:
		updateBody.oldValue.columnValue.intValue = oldValue.columnValue.intValue;
		break;
	case FLOAT:
		updateBody.oldValue.columnValue.floatValue = oldValue.columnValue.floatValue;
		break;
	case TEXT:
		updateBody.oldValue.columnValue.textValue = oldValue.columnValue.textValue;
		break;
	default: break;
	}
	if (update(&updateBody))
		return -1;
	freeValue(&oldValue);
	for (i=0;i<=columnAmount;i++)
	{
		freeValue(&newValues[i]);
	}

	return 0;
}
/*
 *功能：
 *		处理insert命令
 */
int insertCmd()
{//int insert(char *tableName, char **columnsName, Value *values, int amount);
	char tableName[NAME_LENGHT];
	char columnsName[COL_NUM][NAME_LENGHT];
	char *columns[COL_NUM];
	Value values[COL_NUM];
	int amount, i;
	int colFlag=0;

	scaner();
	if (syn!=SYN_INTO)//into
		return -1;
	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;
	if (!checkName(word)) //名，英文字符，下划线
		return -1;
	strcpy(tableName, word);

	scaner();
	if (syn==SYN_VALUES)//values
	{
		colFlag = 1;
	} else if (syn==SYN_PAREN_LEFT)//(
	{
		scaner();
		if (syn!=SYN_IDENTIFIER) //columnsNames
			return -1;
		if (!checkName(word)) //名，英文字符，下划线
			return -1;
		strcpy(columnsName[0], word);
		for (scaner(),amount = 0;syn==SYN_COMMA;scaner()) //,.一直到:)
		{		
			scaner();
			if (syn!=SYN_IDENTIFIER||isKeywords(word))
				return -1;
			if (!checkName(word)) //名，英文字符，下划线
				return -1;
			strcpy(columnsName[++amount],word);
		}
		if (syn!=SYN_PAREN_RIGHT)//)
			return -1;
		scaner();
		if (syn!=SYN_VALUES)//values
			return -1;
	} else if (syn ==SYN_IDENTIFIER)//一个列，省略括号
	{
		colFlag = 2;
		if (!checkName(word)) //名，英文字符，下划线
			return -1;
		strcpy(columnsName[0], word);
		amount = 0;
		scaner();
		if (syn!=SYN_VALUES)//values
			return -1;
	} else
		return -1;
	if (amount==0)
	{
		if (colFlag == 2) {//一个列，省略括号
			if (getValue(&values[0]))
					return -1;
		} else {//一个列，打了括号
			scaner();
			if (syn!=SYN_PAREN_LEFT)//(
			return -1;

			if (getValue(&values[0]))
					return -1;
			
			scaner();
			if (syn!=SYN_PAREN_RIGHT && syn != SYN_SEMICOLON )//),;
				return -1;
		}
	}else{
		scaner();
		if (syn!=SYN_PAREN_LEFT)//(
			return -1;
		for (i=0;1;i++)
		{
			if (getValue(&values[i]))
				return -1;
			if (values[i].columnType==EMPTY)
				continue;
			scaner();
			if (syn == SYN_PAREN_RIGHT || syn == SYN_SEMICOLON)//),;
				break;
			if (syn != SYN_COMMA)//,
				return -1;
		}
		if (!colFlag&&i!=amount)//columns == values
			return -1;
		else 
			amount = i;
		if (syn!=SYN_PAREN_RIGHT && syn != SYN_SEMICOLON)//)
			return -1;
	}

	if ((syn != SYN_SEMICOLON) && !checkEnd())//确认命令结束
		return -1;

	if (colFlag)//all the columns
	{
		if (insert(tableName,NULL,values, amount+1))
			return -1;
	}else{
		for (i=0;i<=amount;i++)
			columns[i] = columnsName[i];
		if (insert(tableName,columns,values, amount+1))
			return -1;
	}
	for (i=0;i<=amount;i++)//释放内存
		freeValue(&values[i]);

	return 0;
}
/*
 *功能：
 *		有了Order命令，设置类型
 */
int setSort(SORT_ORDER *sortOrder, char *sortColumn)
{
	scaner();
	if (syn!=SYN_BY)//by
		return -1;
	scaner();
	if (syn!=SYN_IDENTIFIER)//sort column name
		return -1;
	if (!checkName(word)) //名，英文字符，下划线
		return -1;
	strcpy(sortColumn, word);
	
	scaner();
	if (syn==SYN_DESC)
		*sortOrder = DESC;
	else if (syn==SYN_INCR)
		*sortOrder = INCR;
	else 
		return -1;
	return 0;
}
/*
 *功能：
 *		获得一个where的条件
 */
int setWhere(Condition **condition)
{
	char expStr[1024];
	char *temp = p;//保存当前命令位置
	int num, i;
	(*condition) = (Condition *)malloc(sizeof(Condition));

	if (checkLogic())//复合and,or
	{
		num = 0; //(的数目
		i=0;
		while(isspace(*p)) p++;
		do{
			expStr[i]=p[i];
			if (p[i]=='(')
				num++;
			if (p[i]==')')
				num--;
			if (p[i]==';' || p[i]=='\0')//end
				return -1;
			i++;
		}while(num>0);
		expStr[i]='\0';
		temp = p+i;		
		p = expStr;

		if (setLogicWhere(expStr, condition))
			return -1;	
		p = temp;
	}else {//一个条件
		scaner();
		if (syn!=SYN_IDENTIFIER)//columnName
			return -1;
		if (!checkName(word))
			return -1;
		strcpy((*condition)->columnName, word);

		scaner();
		switch(syn)
		{
		case SYN_EQUAL://==
			(*condition)->operator = EQ;
			break;
		case SYN_NOT_EQUAL://~=
			(*condition)->operator = NE;
			break;
		case SYN_GREATER://>
			(*condition)->operator = GT;
			break;
		case SYN_LESS://<
			(*condition)->operator = LT;
			break;
		case SYN_MORE_EQUAL://>=
			(*condition)->operator = GET;
			break;
		case SYN_LESS_EQUAL://<=
			(*condition)->operator = LET;
			break;
		case SYN_LIKE://like
			(*condition)->operator = LIKE;
			break;
		case SYN_BETWEEN://between
			(*condition)->operator = BETWEEN;
			break;
		default:
			return -1;
			break;
		}
		(*condition)->logic = NOLOGIC;
		if ((*condition)->operator == BETWEEN)//between[value1, value2]
		{
			scaner();
			if (syn!=SYN_BRACKET_LEFT)//[
				return -1;
			if(getValue(&((*condition)->value)))//value1
				return -1;
			scaner();
			if (syn!=SYN_COMMA)//,
				return -1;
			if (getValue(&((*condition)->value2)))//value2
				return -1;
			scaner();
			if (syn!=SYN_BRACKET_RIGHT)//]
				return -1;
		} else
		{
			//getvalue
			if (getValue(&((*condition)->value)))
				return -1;
		}
		(*condition)->next = NULL;
	}
	return 0;
}
/*
 *功能：
 *		检查是否有or,and
 */
int checkLogic()
{
	char *temp = p;
	
	scaner();
	p = temp;
	if (syn == SYN_PAREN_LEFT)//(
		return 1;
	return 0;//没有
}
/*
 *功能：
 *		处理select命令
 */
int selectCmd(int isInner, Value *resultValue)
{//int select(SelectBody *selectBody);
	SelectBody selectBody;
	char columnsName[COL_NUM][NAME_LENGHT];
	char *colums[COL_NUM];
	char tableName[NAME_LENGHT];	
	int columnAmount, i;
	
	char sortColumnName[NAME_LENGHT];
	SORT_ORDER sortOrder = NOTSORT;
	Condition *conditon;
	Condition *temp;
	char *q;

	scaner();
	if (syn==SYN_MULT) //*
	{
		selectBody.columnsName = NULL;
		selectBody.columnAmount = 0;
		scaner();
		if (syn!=SYN_FROM)//from
			return -1;
	} else if (syn==SYN_IDENTIFIER)//column_name
	{
		for(columnAmount=0;1;scaner(),columnAmount++){
			if (syn!=SYN_IDENTIFIER)//column_name
				return -1;
			strcpy(columnsName[columnAmount], word);
			scaner();
			if (syn==SYN_FROM)//from
				break;
			if (syn!=SYN_COMMA)//,
				return -1;
		}
		columnAmount++;
		for (i=0;i<columnAmount;i++)
			colums[i] = columnsName[i];
		selectBody.columnsName = colums;
		selectBody.columnAmount = columnAmount;
	} else
		return -1;

	scaner();
	if (syn!=SYN_IDENTIFIER)//table_name
		return -1;
	if (!checkName(word))
		return -1;
	strcpy(tableName, word);

	scaner();
	if (syn == SYN_WHERE)//where
	{	
		if (setWhere(&conditon))
			return -1;
		selectBody.condition = conditon; //设置选择条件
		q = p;
		scaner();
		if (syn==SYN_ORDER){
			if (setSort(&sortOrder, sortColumnName))
				return -1;
			scaner();
		}else
			sortOrder = NOTSORT;
		if (!isInner && syn!=SYN_SEMICOLON)//end
			return -1;
		if (isInner && syn != SYN_PAREN_RIGHT)//)
			p = q;
	} else if (syn == SYN_ORDER)//order
	{
		selectBody.condition = NULL;//没有条件
		if (setSort(&sortOrder, sortColumnName))
			return -1;
		if (!checkEnd())//end
				return -1;
	} else if (syn== SYN_SEMICOLON) {//end, ;
		selectBody.condition = NULL;
		sortOrder  = NOTSORT;
	} else 
		return -1;
 
	//todo:select
	selectBody.tableName = tableName;
	if (sortOrder == NOTSORT)
		selectBody.sortColumnName = NULL;
	else 
		selectBody.sortColumnName = sortColumnName;
	selectBody.sortOrder = sortOrder;	
	selectBody.isInner = isInner;
	selectBody.resultValue = resultValue;

	if (select(&selectBody))
		return -1;
	
	if (resultValue!=NULL && !isInner)
		freeValue(resultValue);
	for (temp=selectBody.condition; temp!=NULL; temp=temp->next)
		freeValue(&(temp->value));
	
	return 0;
}
/*
 *功能：
 *		处理一条命令cmd
 */
int processCmd(char *cmd)
{	
	int flag;
	for (p=cmd,flag=0;p!=NULL;)
	{
		scaner();
		if (strlen(word)<1)
			continue;
		switch(syn){
		case SYN_CREATE:   //create
			flag = createCmd();
			break;
		case SYN_ALTER:   //alter
			flag = alterCmd();
			break;
		case SYN_TRUNCATE:  //truncate
			flag = truncateCmd();
			break;
		case SYN_USE: //use
			flag = useCmd();
			break;
		case SYN_DROP: //drop
			flag = dropCmd();
			break;
		case SYN_RENAME://rename
			flag = renameCmd();
			break;
		case SYN_SHOW://show
			flag = showCmd();
			break;
		case SYN_DELETE: //delete
			flag = deleteCmd();
			break;
		case SYN_UPDATE: //update
			flag = updateCmd();
			break;
		case SYN_INSERT: //insert
			flag = insertCmd();
			break;
		case SYN_SELECT: //select
			flag = selectCmd(0, NULL);//外层
			break;
		case SYN_SEMICOLON://;
			flag = 0;
			break;
		default: //unknown cmd
			flag = -1;
			break;
		}
	if (!syn || flag==-1) //end
		break;
	}
	return flag;
}

/*
 *功能：
 *		获得下一个词的种别码
 */
int nextSyn()
{
	char *temp = p;
	scaner();
	p = temp;
	return syn;
}
/*
 *功能：
 *		语法分析中的expression
 *     expression -> term {or term}
 */
int expression(Node *T)
{
	NodeValue value;

	value.nodeType = EMP;
	if(term(setChildTree(T, &value)))
		return -1;

	while(nextSyn() == SYN_OR)//OR
	{
		scaner();
		T->nodeValue.logic= OR;
		T->nodeValue.nodeType = LOG;
		if (term(setBrotherTree(T, &value)))
			return -1;		
	}
	return 0;
}
/*
 *功能：
 *		语法分析中的term
 *      term -> factor {and factor}
 */
int term(Node *T)
{
	NodeValue value;

	value.nodeType = EMP;
	if(factor(setChildTree(T, &value)))
		return -1;

	while(nextSyn() == SYN_AND)//AND
	{
		scaner();
		T->nodeValue.logic= AND;
		T->nodeValue.nodeType = LOG;
		if(factor(setBrotherTree(T, &value)))
			return -1;
	}
    return 0;
}
/*
 *功能：
 *		语法分析中的factor
 *     factor -> condition | (expression)
 */
int factor(Node *T)
{
	char *temp = p;
	scaner();
    switch(syn)
	{
    case SYN_IDENTIFIER://conditon 
		T->child = T->brother = NULL;
		T->nodeValue.nodeType = CONDITION;
		p = temp;
		if (setWhere(&(T->nodeValue.condition)))
			return -1;
		break;
    case SYN_PAREN_LEFT://(
        if (expression(T))
			return -1;
		scaner();
        if(syn != SYN_PAREN_RIGHT)//)
			return -1; //less of )
		break;
    default: 
		return -1;
		break;
	}
	return 0;
}
/*
 *功能：
 *		复杂and, or表达式，化为链，去除括号
 */
int setLogicWhere(char *str, Condition **conditon)
{
	Node T;
	Node *temp = &T;
	Node *tail = NULL;
	NodeValue value;

	T.child = T.brother = T.parent = NULL;
	value.nodeType = EMP;
	copyNodeValue(&T, &value);

	if(expression(&T))
		return -1;
	changeTree(&temp);
	cutTree(&temp);
	Link(temp, &tail, conditon);
	return 0;
}
