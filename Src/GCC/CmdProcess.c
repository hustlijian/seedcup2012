/**
 * @file	CmdProcess.c
 * @author  lijian <hustlijian@gmail.com>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * ����Ľ������֣�����������ö�Ӧ�ĵײ�API
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

char *p;  //��ǰ���������ַ�λ��
char word[MAX_WORDLEGTH]; //���浱ǰ�Ļ�õĵ���
int syn;  //���浱ǰ������

/*���ܣ�
 *     �����ִ�Сд���ַ��Ƚ�
 * ������
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
//�ͷ�value�е�textvalue
void freeValue(Value *value)
{
	if (value->columnType == TEXT)
		free(value->columnValue.textValue);
}
/* ���ܣ�
 *		���string������
 * ������
 * @str:  û�пո���ַ���
 * ���أ�
 *		���Σ�������,����0��������ȷ�������ַ�
 */
int getTypeNum(char *str)
{
	int i, flag;
	
	for (i=0, flag=0;str[i]&&(isdigit(str[i])||str[i]=='.');i++)
		if (str[i]=='.'){
			flag++;
		}
	if (str[i]=='\0')//����
	{
		if (flag==1) return SYN_FLOAT_NUMBER; //������
		if (flag>1) return 0;//��������
		return SYN_INTEGER_NUMBER; //����
	} 
	if (isalpha(str[0])||str[0]=='_') //��ʶ��
	{
		for (i=0;i<NUM_KEYWORDS;i++)
			if (!mystrcmp(keywords[i], str))
				break;
		if (i==NUM_KEYWORDS) return SYN_IDENTIFIER;//һ���ʶ��
		else return (i+1);
	}
	//�����
	for (i=0;i<NUM_OPERATOR;i++)
		if (!strcmp(str, operatorwords[i]))
			break;
	if (i<NUM_OPERATOR) return (i+SYN_OPERATOR_BASE);
	else return 0;
	return 0;
}
/*
 *���ܣ�
 *		��ȡֵ����
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
	default:	//�����Ϊ��ȡ��warning����ʵ����Ҫ
		return EMPTY;
	}
}
/*
 *���ܣ�
 *		��ȡ����������
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
	default:	//�����Ϊ��ȡ��warning����ʵ����Ҫ
		return EQ;
	}
}
/*
 *���ܣ�
 *		�ж��Ƿ��ǲ�����
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
 *���ܣ�
 *		��ȡ�߼�����
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
 *����:
 *     ɨ���ַ���p������һ���ʵ�word
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
	while((isspace(ch=p[i++]))) ; /* Խ�����ַ�*/
	if (ch&&(isalnum(ch)||ch=='_'||ch=='.'))  /*���ʻ�����*/
	{
		j = 0;
		while(ch&&(isalnum(ch)||ch=='_'||ch=='.')) {
			word[j++] = ch;
			ch = p[i++];
		}
		word[j] = '\0';
		syn = getTypeNum(word); 
	}
	else if(ch){   //�������
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
	} else //��
	{
		syn = SYN_ELSE;
		word[0]='\0';
	}
	p = p+i-1;
	if (*p=='\0')
		p=NULL;
}
/*
 *���ܣ�
 *		�ж��Ƿ��ǹؼ���
 */
int isKeywords(char *str)
{
	int i;
	for (i=0;i<NUM_KEYWORDS;i++)
		if (!mystrcmp(str, keywords[i]))
			return 1;
	return 0;  //���ǹؼ���
}
/*
 *���ܣ�
 *		�ж��Ƿ��Ƿ������������ݿ���������������
 */
int checkName(char *str)
{
	int i;
	for (i=0;str[i];i++)
		if(!isalpha(str[i])&&str[i]!='_')	//isalpha�ж��ַ��Ƿ�ΪӢ����ĸ
			return 0;
	return 1;
}
/*
 *���ܣ�
 *		ȷ�ϵ���ĩβ
 */
int checkEnd()
{
	scaner();
	if (syn==SYN_SEMICOLON || word[0]=='\0')
		return 1;
	return 0;
}
/*
 *���ܣ�
 *		�������ݿ�����
 */
int createDatabaseCmd()
{
	char databaseName[NAME_LENGHT];

	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word)) //���ؼ���
		return -1;
	if (!checkName(word)) //�����������Ӣ���ַ����»���
		return -1;
	strcpy(databaseName, word);

	if (!checkEnd())//ȷ���������
		return -1;
	if (createDatabase(databaseName))//�������ݿ�
		return -1;
	return 0;
}
/*
 *���ܣ�
 *		����������
 */
int createTableCmd()
{
	int countAmount, i;
	char columnsName[COL_NUM][NAME_LENGHT];
	char tableName[NAME_LENGHT];
	COLUMN_TYPE columnType[COL_NUM];
	char *p[COL_NUM];

	scaner();
	if (syn!=SYN_IDENTIFIER || isKeywords(word))//����ʹ�ùؼ���
		return -1;
	if (!checkName(word)) //�����������Ӣ���ַ����»���
		return -1;
	strcpy(tableName,word);


	if (checkEnd())//���б�
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
		if (!checkName(word)) //�����������Ӣ���ַ����»���
			return -1;
		strcpy(columnsName[countAmount], word);

		scaner();
		if(syn>=SYN_INT&&syn<=SYN_NONE)  //ָ������int26,float27,text28
			columnType[countAmount] = syn-SYN_INT;
		else {
			columnType[countAmount] = NONE;
			if (syn==SYN_PAREN_RIGHT) break; //')'
			if (syn==SYN_COMMA) continue; //����
			else return -1;
		}

		scaner();
		if (syn==SYN_PAREN_RIGHT) break; //������
		if (syn!=SYN_COMMA) return -1; //����
	}

	for (i=0;i<=countAmount;i++)
	{
		p[i]=columnsName[i];
	}
	
	if (!checkEnd())//ȷ���������
		return -1;
	if(createTable(tableName, p, columnType, countAmount+1))
		return -1;
	return 0;
}
/*
 *���ܣ�
 *		�������ݿ���߱�
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
 *���ܣ�
 *		������
 */
int alterAddCmd(char *tableName)
{
	char columnName[NAME_LENGHT];
	COLUMN_TYPE columnType;

	scaner();
	if (syn != SYN_IDENTIFIER)
		return -1;
	if (!checkName(word)) //����Ӣ���ַ����»���
		return -1;
	strcpy(columnName,word);

	scaner();
	if(syn>=SYN_INT&&syn<=SYN_TEXT)  //ָ������26,27,28
		columnType = syn-SYN_INT;
	else if (syn==SYN_SEMICOLON) 
		columnType = NONE;
	else
		return -1;

	if (!checkEnd())//ȷ���������
		return -1;

	if (addColumn(tableName,columnName,columnType))
		return -1;
	return 0;		
}
/*
 *���ܣ�
 *		ɾ����
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
	if (!checkName(word)) //����Ӣ���ַ����»���
		return -1;
	strcpy(columnName,word);

	if (!checkEnd())//ȷ���������
		return -1;
	if (rmColumn(tableName,columnName))
		return -1;
	return 0;
}
/*
 *���ܣ�
 *		�޸�������
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
	if (!checkName(word)) //����Ӣ���ַ����»���
		return -1;
	strcpy(columnName,word);
	
	scaner();
	if(syn>=SYN_INT&&syn<=SYN_TEXT)  //ָ������26,27,28
		columnType = syn-SYN_INT;
	else if (syn==SYN_SEMICOLON) 
		columnType = NONE;
	else
		return -1;

	if (!checkEnd())//ȷ���������
		return -1;
	if (alterColumn(tableName,columnName,columnType))
		return -1;

	return 0;	

}
/*
 *���ܣ�
 *		�޸���
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
	if (!checkName(word)) //����Ӣ���ַ����»���
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
 *���ܣ�
 *		���������
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
	if (!checkName(word)) //����Ӣ���ַ����»���
		return -1;
	strcpy(tableName, word);

	if (!checkEnd())//ȷ���������
		return -1;
	if (truncateTable(tableName))
		return -1;
	return 0;
}
/*
 *���ܣ�
 *		ָ�����ݿ�
 */
int useCmd()
{
	char databaseName[NAME_LENGHT];
	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;
	if (!checkName(word)) //����Ӣ���ַ����»���
		return -1;
	strcpy(databaseName, word);

	if (!checkEnd())//ȷ���������
		return -1;
	if (use(databaseName))
		return -1;
	return 0;
}
/*
 *���ܣ�
 *		ɾ���������ݿ�
 */
int dropCmd()
{
	char databaseName[NAME_LENGHT];
	char tableName[NAME_LENGHT];

	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;
	if (!checkName(word)) //����Ӣ���ַ����»���
		return -1;
	strcpy(databaseName, word);

	scaner();
	if (syn==SYN_SEMICOLON)
		return drop(databaseName, NULL);
	if (syn==SYN_IDENTIFIER){
		if (!checkName(word)) //����Ӣ���ַ����»���
			return -1;
		strcpy(tableName, word);
	}
	else
		return -1;

	if (!checkEnd())//ȷ���������
		return -1;
	if(drop(databaseName,tableName)) 
			return -1;
	return 0;
}
/*
 *���ܣ�
 *		���������ݿ�
 */
int renameDatabaseCmd()
{
	char oldName[NAME_LENGHT];
	char newName[NAME_LENGHT];

	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;
	if (!checkName(word)) //����Ӣ���ַ����»���
		return -1;
	strcpy(oldName, word);

	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;
	if (!checkName(word)) //����Ӣ���ַ����»���
		return -1;
	strcpy(newName, word);
	
	if (!checkEnd())//ȷ���������
		return -1;
	if (renameDatabase(oldName, newName))
		return -1;
	return 0;
}
/*
 *���ܣ�
 *		��������
 */
int renameTableCmd()
{
	char oldName[NAME_LENGHT];
	char newName[NAME_LENGHT];
	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;
	if (!checkName(word)) //����Ӣ���ַ����»���
		return -1;
	strcpy(oldName, word);
	
	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;
	if (!checkName(word)) //����Ӣ���ַ����»���
		return -1;
	strcpy(newName, word);
	
	if (!checkEnd())//ȷ���������
		return -1;
	if (renameTable(oldName, newName))
		return -1;
	return 0;
}
/*
 *���ܣ�
 *		����������
 */
int renameCmd()
{
	scaner();
	if (syn==SYN_DATABASE)//���ݿ�
	{
		return renameDatabaseCmd();
	} else if(syn==SYN_TABLE)//��
	{
		return renameTableCmd();
	}
	else return -1;
}
/*
 *���ܣ�
 *		ȷ�����������
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
 *���ܣ�
 *		��ʾ���ݿ�
 */
int showDatabaseCmd()
{
	SORT_ORDER sortOrder;

	if (checkSort(&sortOrder))
		return -1;		
	
	if (!checkEnd())//ȷ���������
		return -1;
	if(showDatabase(sortOrder))
		return -1;
	return 0;
}
/*
 *���ܣ�
 *		��ʾ���ݱ���
 */
int showTableCmd(char *databaseName)
{//showTable(char *databaseName,SORT_ORDER sortOrder);  //databaseNameΪNULLʱָ��Ϊ��ǰ���ݿ�
	SORT_ORDER sortOrder;

	if (checkSort(&sortOrder))
		return -1;

	if (!checkEnd())//ȷ���������
		return -1;
	if(showTable(databaseName, sortOrder))
		return -1;
	return 0;
}
/*
 *���ܣ�
 *		��ʾ����
 */
int showColumnCmd(char *tableName)
{//showColumn(char *tableName, SORT_ORDER sortOrder);
	SORT_ORDER sortOrder;
	
	if (checkSort(&sortOrder))
		return -1;
	
	if (!checkEnd())//ȷ���������
		return -1;
	if(showColumn(tableName, sortOrder))
		return -1;
	return 0;
}
/*
 *���ܣ�
 *		������ʾ
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
		if (!checkName(word)) //����Ӣ���ַ����»���
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
 *���ܣ�
 *		Ϊvalue��ֵ
 */
int getValue(Value *value)
{
	char *str;
	int i,num;
	char innerSelect[1024];
	char *temp = p;//���浱ǰ����λ��

	scaner();
	if (syn==SYN_QUOTE)//',�������ַ���
	{
		value->columnType = TEXT;
		str = (char *)malloc(sizeof(char)*NAME_LENGHT);
		if (str==NULL)
			return -1;
		i=0; 
		do {
			str[i]=p[i];
			if(!isalnum(str[i]) && str[i]!=' ' && str[i] != '_' && str[i]!='\'')
			{	//�������ַ����֣����߿ո��»���,������ 
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
	} else if (syn==SYN_INTEGER_NUMBER)//����
	{
		value->columnType = INT;
		value->columnValue.intValue = atoi(word);
		return 0;
	}else if (syn==SYN_FLOAT_NUMBER)//������
	{
		value->columnType = FLOAT;
		value->columnValue.floatValue = (float)atof(word);
		return 0;
	} else if (syn==SYN_COMMA)//,.������
	{
		value->columnType = EMPTY;
		return 0;
	} else if (syn==SYN_BRACKET_LEFT)//[,like�е�text����
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
			{	//�������ַ����֣����߿ո��»���,������ ,�ʺţ��Ǻ�
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
			if (selectCmd(1, value))//�ڲ���ѯ
				return -1;
		} else if (syn==SYN_INTEGER_NUMBER || syn== SYN_FLOAT_NUMBER || 
			syn == SYN_PAREN_LEFT || syn == SYN_MIMUS || syn== SYN_ADD)//int,float,(,-,+
		{
			num = 0; //(����Ŀ
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
 *���ܣ�
 *		ɾ������
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
	if (!checkName(word)) //����Ӣ���ַ����»���
		return -1;
	strcpy(tableName, word);

	scaner();
	if (syn!=SYN_WHERE)//where
		return -1;

	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;
	if (!checkName(word)) //����Ӣ���ַ����»���
		return -1;
	strcpy(colum, word);

	scaner();
	if (syn!=SYN_EQUAL)//==
		return -1;

	if (getValue(&value))
		return -1;

	if (!checkEnd())//ȷ���������
		return -1;
	if (delete(tableName, colum, value))
		return -1;
	freeValue(&value);

	return 0;
}
/*
 *���ܣ�
 *		����update����
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
	if (!checkName(word)) //����Ӣ���ַ����»���
		return -1;
	strcpy(tableName, word);

	scaner();
	if (syn!=SYN_SET)//set
		return -1;

	scaner();
	if (syn==SYN_IDENTIFIER)//һ�У�û������
	{
		flag = 1; //û������
		if (!checkName(word)) //����Ӣ���ַ����»���
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
	if (!checkName(word)) //����Ӣ���ַ����»���
		return -1;
    strcpy(columnsName[0], word);
    for (scaner(),columnAmount = 0;syn==SYN_COMMA;scaner()) //һֱ��:)
    {		
      scaner();
      if (syn!=SYN_IDENTIFIER||isKeywords(word))
        return -1;
	  if (!checkName(word)) //����Ӣ���ַ����»���
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
	  if (flag) //û������
	  {
		if (getValue(&newValues[0]))//һ�У�û������
			return -1;
	  } else {
		  scaner();
		  if (syn != SYN_PAREN_LEFT)//(
			return -1;
		  if (getValue(&newValues[0]))//һ�У�û������
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
	if (!checkName(word)) //����Ӣ���ַ����»���
		return -1;
	strcpy(updatedColumn, word);

	scaner();
	if (syn!=SYN_EQUAL)//==
		return -1;
	if (getValue(&oldValue))
		return -1;
	
	if (!checkEnd())//ȷ���������
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
 *���ܣ�
 *		����insert����
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
	if (!checkName(word)) //����Ӣ���ַ����»���
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
		if (!checkName(word)) //����Ӣ���ַ����»���
			return -1;
		strcpy(columnsName[0], word);
		for (scaner(),amount = 0;syn==SYN_COMMA;scaner()) //,.һֱ��:)
		{		
			scaner();
			if (syn!=SYN_IDENTIFIER||isKeywords(word))
				return -1;
			if (!checkName(word)) //����Ӣ���ַ����»���
				return -1;
			strcpy(columnsName[++amount],word);
		}
		if (syn!=SYN_PAREN_RIGHT)//)
			return -1;
		scaner();
		if (syn!=SYN_VALUES)//values
			return -1;
	} else if (syn ==SYN_IDENTIFIER)//һ���У�ʡ������
	{
		colFlag = 2;
		if (!checkName(word)) //����Ӣ���ַ����»���
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
		if (colFlag == 2) {//һ���У�ʡ������
			if (getValue(&values[0]))
					return -1;
		} else {//һ���У���������
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

	if ((syn != SYN_SEMICOLON) && !checkEnd())//ȷ���������
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
	for (i=0;i<=amount;i++)//�ͷ��ڴ�
		freeValue(&values[i]);

	return 0;
}
/*
 *���ܣ�
 *		����Order�����������
 */
int setSort(SORT_ORDER *sortOrder, char *sortColumn)
{
	scaner();
	if (syn!=SYN_BY)//by
		return -1;
	scaner();
	if (syn!=SYN_IDENTIFIER)//sort column name
		return -1;
	if (!checkName(word)) //����Ӣ���ַ����»���
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
 *���ܣ�
 *		���һ��where������
 */
int setWhere(Condition **condition)
{
	char expStr[1024];
	char *temp = p;//���浱ǰ����λ��
	int num, i;
	(*condition) = (Condition *)malloc(sizeof(Condition));

	if (checkLogic())//����and,or
	{
		num = 0; //(����Ŀ
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
	}else {//һ������
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
 *���ܣ�
 *		����Ƿ���or,and
 */
int checkLogic()
{
	char *temp = p;
	
	scaner();
	p = temp;
	if (syn == SYN_PAREN_LEFT)//(
		return 1;
	return 0;//û��
}
/*
 *���ܣ�
 *		����select����
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
		selectBody.condition = conditon; //����ѡ������
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
		selectBody.condition = NULL;//û������
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
 *���ܣ�
 *		����һ������cmd
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
			flag = selectCmd(0, NULL);//���
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
 *���ܣ�
 *		�����һ���ʵ��ֱ���
 */
int nextSyn()
{
	char *temp = p;
	scaner();
	p = temp;
	return syn;
}
/*
 *���ܣ�
 *		�﷨�����е�expression
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
 *���ܣ�
 *		�﷨�����е�term
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
 *���ܣ�
 *		�﷨�����е�factor
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
 *���ܣ�
 *		����and, or����ʽ����Ϊ����ȥ������
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