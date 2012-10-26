#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "expression.h"
#include "CmdProcess.h"
#include "LogicExpStruct.h"

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
	case SYN_ASSIGN:
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
	case SYN_ASSIGN:
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
int isKeywords(char *str)
{
	int i;
	for (i=0;i<NUM_KEYWORDS;i++)
		if (!mystrcmp(str, keywords[i]))
			return 1;
	return 0;  //不是关键字
}

int checkName(char *str)
{
	int i;
	for (i=0;str[i];i++)
		if(!isalpha(str[i])&&str[i]!='_')	//isalpha判断字符是否为英文字母
			return 0;
	return 1;
}
int createDatabaseCmd()
{
	char databaseName[NAME_LENGHT];

	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word)) //检查关键字
		return -1;
	if (!checkName(word)) //检查数据名，英文字符，下划线
		return -1;
	strcpy(databaseName, word);

	scaner();
	if (syn)//确认命令结束
		return -1;
	if (createDatabase(databaseName))//建立数据库
		return -1;
	return 0;
}
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

	scaner();
	if (syn == 0)//无列表
	{
		if(createTable(tableName,NULL, NULL, 0))
			return -1;
		return 0;
	}
	if(syn!=SYN_PAREN_LEFT)	//'('
		return -1;
	for (countAmount=0;countAmount<COL_NUM;countAmount++)
	{
		scaner();
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
			if (syn!=SYN_COMMA) return -1; //逗号
		}

		scaner();
		if (syn==SYN_PAREN_RIGHT) break; //右括号
		if (syn!=SYN_COMMA) return -1; //逗号
	}

	for (i=0;i<=countAmount;i++)
	{
		p[i]=columnsName[i];
	}
	
	scaner();
	if (syn)//确认命令结束
		return -1;

	if(createTable(tableName, p, columnType, countAmount+1))
		return -1;
	return 0;
}
//建立数据库或者表
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
//添加列
int alterAddCmd(char *tableName)
{
	char columnName[NAME_LENGHT];
	COLUMN_TYPE columnType;

	scaner();
	if (syn != SYN_IDENTIFIER)
		return -1;
	strcpy(columnName,word);

	scaner();
	if(syn>=SYN_INT&&syn<=SYN_FLOAT)  //指定类型26,27,28
		columnType = syn-SYN_INT;
	else if (syn==SYN_ELSE) 
		columnType = NONE;
	else
		return -1;

	scaner();
	if (syn)//确认命令结束
		return -1;

	if (addColumn(tableName,columnName,columnType))
		return -1;
	return 0;		
}
//删除列
int alterRmCmd(char *tableName)
{
	char columnName[NAME_LENGHT];
	scaner();
	if (syn!=SYN_COLUMN)
		return -1;

	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;
	
	strcpy(columnName,word);

	scaner();
	if (syn)//确认命令结束
		return -1;
	if (rmColumn(tableName,columnName))
		return -1;
	return 0;
}
//修改列类型
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
	strcpy(columnName,word);
	
	scaner();
	if(syn>=SYN_INT&&syn<=SYN_TEXT)  //指定类型26,27,28
		columnType = syn-SYN_INT;
	else if (syn==SYN_ELSE) 
		columnType = NONE;
	else
		return -1;
	scaner();
	if (syn)//确认命令结束
		return -1;
	if (alterColumn(tableName,columnName,columnType))
		return -1;

	return 0;	

}
//修改列
int alterCmd()
{
	char tableName[NAME_LENGHT];
	scaner();
	if (syn!=SYN_TABLE)
		return -1;

	scaner();
	if (syn!=SYN_IDENTIFIER)
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

//清除表数据
int truncateCmd()
{
	char tableName[NAME_LENGHT];
	scaner();
	if (syn!=SYN_TABLE)
		return -1;
	
	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;
	strcpy(tableName, word);

	scaner();
	if (syn)//确认命令结束
		return -1;
	if (truncateTable(tableName))
		return -1;
	return 0;
}
//指定数据库
int useCmd()
{
	char databaseName[NAME_LENGHT];
	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;
	strcpy(databaseName, word);

	scaner();
	if (syn)//确认命令结束
		return -1;
	if (use(databaseName))
		return -1;
	return 0;
}
//删除表或数据库
int dropCmd()
{
	char databaseName[NAME_LENGHT];
	char tableName[NAME_LENGHT];

	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;
	strcpy(databaseName, word);

	scaner();
	if (syn==SYN_ELSE)
		return drop(databaseName, NULL);
	if (syn==SYN_IDENTIFIER)
		strcpy(tableName, word);
	else
		return -1;

	scaner();
	if (syn)//确认命令结束
		return -1;
	if(drop(databaseName,tableName)) 
			return -1;
	return 0;
}
//重命名数据库
int renameDatabaseCmd()
{//renameDatabase(char *oldName, char *newName);
	char oldName[NAME_LENGHT];
	char newName[NAME_LENGHT];

	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;
	strcpy(oldName, word);

	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;

	strcpy(newName, word);
	
	scaner();
	if (syn)//确认命令结束
		return -1;
	if (renameDatabase(oldName, newName))
		return -1;
	return 0;
}
//重命名表
int renameTableCmd()
{//renameTable(char *oldName, char *newName);
	char oldName[NAME_LENGHT];
	char newName[NAME_LENGHT];
	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;
	strcpy(oldName, word);
	
	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;
	strcpy(newName, word);
	
	scaner();
	if (syn)//确认命令结束
		return -1;
	if (renameTable(oldName, newName))
		return -1;
	return 0;
}
//处理重命名
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

int  checkSort(SORT_ORDER *sortOrder)
{
	scaner();
	if (syn==SYN_ELSE) //NOSORT
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
//显示数据库
int showDatabaseCmd()
{//showDatabase(SORT_ORDER sortOrder);
	SORT_ORDER sortOrder;

	if (checkSort(&sortOrder))
		return -1;		
	
	scaner();
	if (syn)//确认命令结束
		return -1;
	if(showDatabase(sortOrder))
		return -1;
	return 0;
}
//显示数据表格
int showTableCmd(char *databaseName)
{//showTable(char *databaseName,SORT_ORDER sortOrder);  //databaseName为NULL时指定为当前数据库
	SORT_ORDER sortOrder;

	if (checkSort(&sortOrder))
		return -1;

	scaner();
	if (syn)//确认命令结束
		return -1;
	if(showTable(databaseName, sortOrder))
		return -1;
	return 0;
}
//显示列名
int showColumnCmd(char *tableName)
{//showColumn(char *tableName, SORT_ORDER sortOrder);
	SORT_ORDER sortOrder;
	
	if (checkSort(&sortOrder))
		return -1;
	
	scaner();
	if (syn)//确认命令结束
		return -1;
	if(showColumn(tableName, sortOrder))
		return -1;
	return 0;
}
//处理显示
int showCmd()
{
	char databaseName[NAME_LENGHT];

	scaner();
	if (syn==SYN_DATABASES)//databases
		return showDatabaseCmd();
	else if(syn==SYN_TABLE)//table
		return showTableCmd(NULL);
	else if(syn==SYN_IDENTIFIER)//database_name,table_name
		strcpy(databaseName, word);
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
//为value赋值
int getValue(Value *value)
{
	char *str;
	int i;

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
	} else
		return -1;
} 
//删除命令
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
	strcpy(tableName, word);

	scaner();
	if (syn!=SYN_WHERE)//where
		return -1;

	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;
	strcpy(colum, word);

	scaner();
	if (syn!=SYN_EQUAL)//==
		return -1;

	if (getValue(&value))
		return -1;
	scaner();
	if (syn)//确认命令结束
		return -1;
	if (delete(tableName, colum, value))
		return -1;
	if (value.columnType==TEXT)
		free(value.columnValue.textValue);

	return 0;
}
//处理update命令
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
	int i;

	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;
	strcpy(tableName, word);

	scaner();
	if (syn!=SYN_SET)//set
		return -1;

	scaner();
	if (syn==SYN_IDENTIFIER)
	{
		strcpy(columnsName[0], word);
		columnAmount = 0;
	}else 
	{	
    if (syn!=SYN_PAREN_LEFT)//(
      return -1;
    
    scaner();
    if (syn!=SYN_IDENTIFIER) //columnsNames
      return -1;
    strcpy(columnsName[0], word);
    for (scaner(),columnAmount = 0;syn==SYN_COMMA;scaner()) //一直到:)
    {		
      scaner();
      if (syn!=SYN_IDENTIFIER||isKeywords(word))
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
		if (getValue(&newValues[0]))
			return -1;
	} else
	{
    scaner();
    if (syn!=SYN_PAREN_LEFT)//(
      return -1;
    if (getValue(&newValues[0]))
      return -1;
    for (i=0,scaner();syn==SYN_COMMA;scaner())
    {
      scaner();
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
	strcpy(updatedColumn, word);

	scaner();
	if (syn!=SYN_EQUAL)//==
		return -1;
	if (getValue(&oldValue))
		return -1;
	
	scaner();
	if (syn)//确认命令结束
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
	if (columnType == TEXT)
		free(oldValue.columnValue.textValue);

	return 0;
}
//处理insert命令
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
		strcpy(columnsName[0], word);
		for (scaner(),amount = 0;syn==SYN_COMMA;scaner()) //,.一直到:)
		{		
			scaner();
			if (syn!=SYN_IDENTIFIER||isKeywords(word))
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
		strcpy(columnsName[0], word);
		amount = 0;
		scaner();
		if (syn!=SYN_VALUES)//values
			return -1;
	} else
		return -1;
	if (amount==0)
	{
		if (getValue(&values[0]))
				return -1;
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
			if (syn == SYN_PAREN_RIGHT)//)
				break;
			if (syn != SYN_COMMA)//,
				return -1;
		}
		if (!colFlag&&i!=amount)//columns == values
			return -1;
		if (syn!=SYN_PAREN_RIGHT)//)
			return -1;
	}

	scaner();
	if (syn)//确认命令结束
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
	return 0;
}

int logicalExpProc(char *expStr, Condition **expList)
{
	LogicExpStack lExpTmpStk, lExpPloStk;
	LogicExpStack lExpRevPloStk, lExpRevPloStk2;
	char *e;
	int w;
	Condition *locHeadList, *locTailList;

	locHeadList = (Condition *)malloc(sizeof(Condition));
	locTailList = locHeadList;
	locTailList->next = NULL;
	*expList = locHeadList;
	//e = (char *)calloc(NAME_MAX, sizeof(char));
	if(logicalExpStkInit(&lExpTmpStk))
		return -1;
	if (logicalExpStkInit(&lExpPloStk))
		return -1;
	if (logicalExpStkInit(&lExpRevPloStk))
		return -1;
	if (logicalExpStkInit(&lExpRevPloStk2))
		return -1;
	logicalExpStkPush(&lExpTmpStk, "$", 0);	//$优先级最低
	p=expStr;
	while(p != NULL)
	{
		scaner();
		if (syn == SYN_PAREN_LEFT || syn == SYN_PAREN_RIGHT ||
			syn == SYN_OR || syn == SYN_AND)	//运算符
		{
			switch(syn)
			{
			case SYN_PAREN_LEFT:	//压入lExpTmpStk
				logicalExpStkPush(&lExpTmpStk, word, 1);
				break;
			case SYN_PAREN_RIGHT:
				if (logicalExpStkGetTop(&lExpTmpStk, &e, &w))
					return -1;
				while(strcmp(e, "("))
				{
					if(logicalExpStkPop(&lExpTmpStk, &e, &w))
						return -1;
					logicalExpStkPush(&lExpPloStk, e, 2);
					if(logicalExpStkGetTop(&lExpTmpStk, &e, &w))
						return -1;
				}
				if(logicalExpStkPop(&lExpTmpStk, &e, &w))
					return -1;
				break;
			case SYN_OR:
			case SYN_AND:
				for (; strcmp(e, "$");)
				{
					if(logicalExpStkGetTop(&lExpTmpStk, &e, &w))
						return -1;
					if (!strcmp(e, "("))
						break;
					else
					{
						if(logicalExpStkGetTop(&lExpTmpStk, &e, &w))
							return -1;
						if(logicalExpStkGetTop(&lExpPloStk, &e, &w))
							return -1;
						while (w >= 3 && strcmp(e, "("))
						{
							if(logicalExpStkPop(&lExpTmpStk, &e, &w))
								return -1;
							logicalExpStkPush(&lExpPloStk, e, 3);
							if(logicalExpStkGetTop(&lExpTmpStk, &e, &w))
								return -1;
						}
					}
					if(logicalExpStkGetTop(&lExpTmpStk, &e, &w))
						return -1;
				}
				logicalExpStkPush(&lExpTmpStk, word, 3);
				break;
			default:
				break;
			}
		}
		else	//操作数，即条件，压入lExpPloStk栈
			if(SYN_QUOTE != syn && SYN_BRACKET_LEFT != syn &&
				SYN_BRACKET_RIGHT != syn && SYN_COMMA != syn)
				logicalExpStkPush(&lExpPloStk, word, 4);
	}
	if(logicalExpStkPop(&lExpTmpStk, &e, &w))
		return -1;
	while (strcmp(e, "$"))	//若lExpTmpStk不空，将剩余操作符弹出到lExpPloStk
	{
		if (!strcmp(e, "(") || !strcmp(e, ")"))
			return -1;
		logicalExpStkPush(&lExpPloStk, e, w);
		if (logicalExpStkPop(&lExpTmpStk, &e, &w))
			return -1;	
	}
	//逆序弹出lExpPloStk输出即为逆波兰式
	logicalExpStkPush(&lExpRevPloStk, "$", 0);
	while (logicalExpStkEmpty(&lExpPloStk))
	{
		if (logicalExpStkPop(&lExpPloStk, &e, &w))
			return -1;
		printf("%s ", e);
		logicalExpStkPush(&lExpRevPloStk, e, w);
	}
	//处理逆波兰式，写入链表
	do
	{
		if(logicalExpStkPop(&lExpRevPloStk, &e, &w))
			return -1;
		if (SYN_AND == getTypeNum(e) || 
			SYN_OR == getTypeNum(e) || 
			!strcmp(e, "$"))
		{
			locTailList->logic = getLogicType(e);		//与或逻辑

			if(logicalExpStkPop(&lExpRevPloStk2, &e, &w))//第一个值
				return -1;	
			locTailList->value.columnType = getValueType(e);
			switch(getValueType(e))
			{
			case INT:
				locTailList->value.columnValue.intValue = atoi(e);
				break;
			case FLOAT:
				locTailList->value.columnValue.floatValue = (float)atof(e);
				break;
			case TEXT:
				locTailList->value.columnValue.textValue = e;
				break;
			}
			if(logicalExpStkPop(&lExpRevPloStk2, &e, &w))
				return -1;
			if (isLgcExpOpt(e))		//第二个值，若有
			{
				locTailList->value2.columnType = getValueType(e);
				switch(getValueType(e))
				{
				case INT:
					locTailList->value2.columnValue.intValue = atoi(e);
					break;
				case FLOAT:
					locTailList->value2.columnValue.floatValue = (float)atof(e);
					break;
				case TEXT:
					locTailList->value2.columnValue.textValue = e;
					break;
				}
				if(logicalExpStkPop(&lExpRevPloStk2, &e, &w))
					return -1;
			}
			else
				locTailList->value2.columnType = EMPTY;
			locTailList->operator = getOptType(e);//操作符

			if(logicalExpStkPop(&lExpRevPloStk2, &e, &w))//列名
				return -1;	
			strcpy(locTailList->columnName, e);

			locTailList->next = (Condition *)malloc(sizeof(Condition));
			locTailList = locTailList->next;
			locTailList->next = NULL;
		}
		else
			logicalExpStkPush(&lExpRevPloStk2, e, w);
	}while (logicalExpStkEmpty(&lExpRevPloStk2));
	*expList = locHeadList;
			
	return 0;

}

//处理select命令
int selectCmd(int isInner, Value *resultValue)
{
//select函数接口可能需要重写，由于Select可能产生多行数据，每行数据即是一个Value*数组，
//所以我考虑返回Value*型的二维数组，其中rowAmount指针指向的值记录了行数
//Value*** select(SelectBody *selectBody, int *rowAmount);
	p = NULL;
	return 0;
}
/* 处理一条命令cmd */
int processCmd(char *cmd)
{	
	int flag;
	printf("%s\n", cmd);
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
			//flag = selectCmd();
			break;
		default: //unknown cmd
			flag = -1;
			break;
		}
	if (!syn) //end
		break;
	}
	return flag;
}
