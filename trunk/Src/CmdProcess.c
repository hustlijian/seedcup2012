#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "CmdProcess.h"
#include "DatabaseAPI.h"

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
	} while ((c1==c2)&&c1);
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
		if (flag==1) return 42; //浮点数
		if (flag>1) return 0;//错误输入
		return 41; //整数
	} 
	if (isalpha(str[0])||str[0]=='_') //标识符
	{
		for (i=0;i<NUM_KEYWORDS;i++)
			if (!mystrcmp(keywords[i], str))
				break;
		if (i==NUM_KEYWORDS) return 40;//一般标识符
		else return (i+1);
	}
	//运算符
	for (i=0;i<NUM_OPERATOR;i++)
		if (!strcmp(str, operatorwords[i]))
			break;
	if (i<NUM_OPERATOR) return (i+50);
	else return 0;
	return 0;
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
		syn = 0;
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
		syn = 0;
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

int checkDatabaseName(char *str)
{
	int i;
	for (i=0;str[i];i++)
		if(!isalpha(str[i])&&str[i]!='_')
			return 0;
	return 1;
}
int createDatabaseCmd()
{
	char databaseName[NAME_LENGHT];
	scaner();
	if (syn!=40||isKeywords(word)) //不是关键字
		return -1;
	if (!checkDatabaseName(word)) //检查数据名，英文字符，下划线
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
	if (syn!=40 || isKeywords(word))//表名使用关键字
		return -1;
	if (!checkDatabaseName(word)) //检查数据名，英文字符，下划线
		return -1;
	strcpy(tableName,word);

	scaner();
	if(syn!=50)
		return -1;
	for (countAmount=0;countAmount<COL_NUM;countAmount++)
	{
		scaner();
		if(syn!=40||isKeywords(word)) 
			return -1;
		if (!checkDatabaseName(word)) //检查数据名，英文字符，下划线
			return -1;
		strcpy(columnsName[countAmount],word);

		scaner();
		if(syn>25&&syn<29)  //指定类型26,27,28
			columnType[countAmount] = syn-26;
		else {
			columnType[countAmount] = NONE;
			if (syn==51) break; //右括号
			if (syn!=66) return -1; //逗号
		}

		scaner();
		if (syn==51) break; //右括号
		if (syn!=66) return -1; //逗号
	}

	for (i=0;i<=countAmount;i++)
	{
		p[i]=columnsName[i];
	}
	
	scaner();
	if (syn)//确认命令结束
		return -1;

	if(createTable(tableName,p, columnType, countAmount+1))
		return -1;
	return 0;
}
//建立数据库或者表
int createCmd()
{
	scaner();
	if(syn==2)
	{
		return createDatabaseCmd();
	} else if (syn==3)
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
	if (syn!=40)
		return -1;
	strcpy(columnName,word);

	scaner();
	if(syn>25&&syn<29)  //指定类型26,27,28
		columnType = syn-26;
	else if (syn==0) 
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
	if (syn!=7)
		return -1;

	scaner();
	if (syn!=40||isKeywords(word))
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
	if (syn!=7)
		return -1;

	scaner();
	if (syn!=40)
		return -1;
	strcpy(columnName,word);
	
	scaner();
	if(syn>25&&syn<29)  //指定类型26,27,28
		columnType = syn-26;
	else if (syn==0) 
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
	if (syn!=3)
		return -1;

	scaner();
	if (syn!=40)
		return -1;
	strcpy(tableName,word);

	scaner();
	if (syn==6)//add
	{
		return alterAddCmd(tableName);
	} else if (syn==9)//drop
	{
		return alterRmCmd(tableName);
	}else if (syn ==4)//alter
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
	if (syn!=3)
		return -1;
	
	scaner();
	if (syn!=40||isKeywords(word))
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
	if (syn!=40||isKeywords(word))
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
	if (syn!=40||isKeywords(word))
		return -1;
	strcpy(databaseName, word);

	scaner();
	if (syn==0)
		return drop(databaseName, NULL);
	if (syn==40)
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
	if (syn!=40||isKeywords(word))
		return -1;
	strcpy(oldName, word);

	scaner();
	if (syn!=40||isKeywords(word))
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
	if (syn!=40||isKeywords(word))
		return -1;
	strcpy(oldName, word);
	
	scaner();
	if (syn!=40||isKeywords(word))
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
	if (syn==2)//数据库
	{
		return renameDatabaseCmd();
	} else if(syn==3)//表
	{
		return renameTableCmd();
	}
	else return -1;
}

int  checkSort(SORT_ORDER *sortOrder)
{
	scaner();
	if (syn==0) //NOSORT
		*sortOrder = NOTSORT;
	else if (syn==14)//SORT
	{
		scaner();
		if (syn!=15)//by
			return -1;
		scaner();
		if (syn==16)
			*sortOrder = DESC;
		else if (syn==17)
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
	if (syn==25)//databases
		return showDatabaseCmd();
	else if(syn==3)//table
		return showTableCmd(NULL);
	else if(syn==40)//database_name,table_name
		strcpy(databaseName, word);
	else
		return -1;

	scaner();
	if (syn==3)//table
		return showTableCmd(databaseName);
	else if (syn==7)//column
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
	if (syn==67)//',类型是字符串
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

	} else if (syn==41)//整数
	{
		value->columnType = INT;
		value->columnValue.intValue = atoi(word);
		return 0;
	}else if (syn==42)//浮点数
	{
		value->columnType = FLOAT;
		value->columnValue.floatValue = (float)atof(word);
		return 0;
	} else if (syn==66)//,.空类型
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
	if (syn!=12)//from
		return -1;

	scaner();
	if (syn!=40||isKeywords(word))
		return -1;
	strcpy(tableName, word);

	scaner();
	if (syn!=13)//where
		return -1;

	scaner();
	if (syn!=40||isKeywords(word))
		return -1;
	strcpy(colum, word);

	scaner();
	if (syn!=57)//==
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
	if (syn!=40||isKeywords(word))
		return -1;
	strcpy(tableName, word);

	scaner();
	if (syn!=19)//set
		return -1;
	scaner();
	if (syn==40)
	{
		strcpy(columnsName[0], word);
		columnAmount = 0;
	}else 
	{	
		if (syn!=50)//(
			return -1;
		
		scaner();
		if (syn!=40) //columnsNames
			return -1;
		strcpy(columnsName[0], word);
		for (scaner(),columnAmount = 0;syn==66;scaner()) //一直到:)
		{		
			scaner();
			if (syn!=40||isKeywords(word))
				return -1;
			strcpy(columnsName[++columnAmount],word);
		}
		if (syn!=51)//)
			return -1;
	}
	scaner();
	if (syn!=69)//=
		return -1;

	if (columnAmount==0)
	{
		if (getValue(&newValues[0]))
			return -1;
	} else
	{
		scaner();
		if (syn!=50)//(
			return -1;
		if (getValue(&newValues[0]))
			return -1;
		for (i=0,scaner();syn==66;scaner())
		{
			scaner();
			if (getValue(&newValues[++i]))
				return -1;
		}
		if (i!=columnAmount)//columns == values
			return -1;
		if (syn!=51)//)
			return -1;
	}
	scaner();
	if (syn!=13)//where
		return -1;

	scaner();
	if (syn!=40||isKeywords(word))
		return -1;
	strcpy(updatedColumn, word);

	scaner();
	if (syn!=57)//==
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
	if (syn!=22)//into
		return -1;
	scaner();
	if (syn!=40||isKeywords(word))
		return -1;
	strcpy(tableName, word);

	scaner();
	if (syn==23)//values
	{
		colFlag = 1;
	} else if (syn==50)//(
	{
		scaner();
		if (syn!=40) //columnsNames
			return -1;
		strcpy(columnsName[0], word);
		for (scaner(),amount = 0;syn==66;scaner()) //,.一直到:)
		{		
			scaner();
			if (syn!=40||isKeywords(word))
				return -1;
			strcpy(columnsName[++amount],word);
		}
		if (syn!=51)//)
			return -1;
		scaner();
		if (syn!=23)//values
			return -1;
	} else
		return -1;
	
	scaner();
	if (syn!=50)//(
		return -1;
	for (i=0;1;i++)
	{
		if (getValue(&values[i]))
			return -1;
		if (values[i].columnType==EMPTY)
			continue;
		scaner();
		if (syn == 51)//)
			break;
		if (syn != 66)//,
			return -1;
	}
	if (i!=amount)//columns == values
		return -1;
	if (syn!=51)//)
		return -1;

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

//处理select命令
int selectCmd()
{
//select函数接口可能需要重写，由于Select可能产生多行数据，每行数据即是一个Value*数组，
//所以我考虑返回Value*型的二维数组，其中rowAmount指针指向的值记录了行数
//int select(SelectBody *selectBody, int *rowAmount);
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
		case 1:   //create
			flag = createCmd();
			break;
		case 4:   //alter
			flag = alterCmd();
			break;
		case 5:  //truncate
			flag = truncateCmd();
			break;
		case 8: //use
			flag = useCmd();
			break;
		case 9: //drop
			flag = dropCmd();
			break;
		case 10://rename
			flag = renameCmd();
			break;
		case 24://show
			flag = showCmd();
			break;
		case 20: //delete
			flag = deleteCmd();
			break;
		case 18: //update
			flag = updateCmd();
			break;
		case 21: //insert
			flag = insertCmd();
			break;
		case 11: //select
			flag = selectCmd();
			break;
		default: 
			break;
		}

	}
	return flag;
}
/*用来测试的程序*/
void test()
{
	freopen("input.txt", "r", stdin); 
	freopen("out.txt","w", stdout);
}
