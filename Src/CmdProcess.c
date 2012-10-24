#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "CmdProcess.h"
#include "expression.h"
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

char *p;  //��ǰ�������ַ�λ��
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

/* ���ܣ�
 *		���string������
 * ������
 * @str:  û�пո���ַ���
 * ���أ�
 *		���Σ�������,����0������ȷ�������ַ�
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
int isKeywords(char *str)
{
	int i;
	for (i=0;i<NUM_KEYWORDS;i++)
		if (!mystrcmp(str, keywords[i]))
			return 1;
	return 0;  //���ǹؼ���
}

int checkName(char *str)
{
	int i;
	for (i=0;str[i];i++)
		if(!isalpha(str[i])&&str[i]!='_')	//isalpha�ж��ַ��Ƿ�ΪӢ����ĸ
			return 0;
	return 1;
}
int createDatabaseCmd()
{
	char databaseName[NAME_LENGHT];

	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word)) //���ؼ���
		return -1;
	if (!checkName(word)) //�����������Ӣ���ַ����»���
		return -1;
	strcpy(databaseName, word);

	scaner();
	if (syn)//ȷ���������
		return -1;
	if (createDatabase(databaseName))//�������ݿ�
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
	if (syn!=SYN_IDENTIFIER || isKeywords(word))//����ʹ�ùؼ���
		return -1;
	if (!checkName(word)) //�����������Ӣ���ַ����»���
		return -1;
	strcpy(tableName,word);

	scaner();
	if (syn == 0)//���б�
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
		if (!checkName(word)) //�����������Ӣ���ַ����»���
			return -1;
		strcpy(columnsName[countAmount], word);

		scaner();
		if(syn>=SYN_INT&&syn<=SYN_NONE)  //ָ������int26,float27,text28
			columnType[countAmount] = syn-SYN_INT;
		else {
			columnType[countAmount] = NONE;
			if (syn==SYN_PAREN_RIGHT) break; //')'
			if (syn!=SYN_COMMA) return -1; //����
		}

		scaner();
		if (syn==SYN_PAREN_RIGHT) break; //������
		if (syn!=SYN_COMMA) return -1; //����
	}

	for (i=0;i<=countAmount;i++)
	{
		p[i]=columnsName[i];
	}
	
	scaner();
	if (syn)//ȷ���������
		return -1;

	if(createTable(tableName, p, columnType, countAmount+1))
		return -1;
	return 0;
}
//�������ݿ���߱�
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
//�����
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
	if(syn>=SYN_INT&&syn<=SYN_FLOAT)  //ָ������26,27,28
		columnType = syn-SYN_INT;
	else if (syn==SYN_ELSE) 
		columnType = NONE;
	else
		return -1;

	scaner();
	if (syn)//ȷ���������
		return -1;

	if (addColumn(tableName,columnName,columnType))
		return -1;
	return 0;		
}
//ɾ����
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

	scaner();
	if (syn)//ȷ���������
		return -1;
	if (rmColumn(tableName,columnName))
		return -1;
	return 0;
}
//�޸�������
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
	else if (syn==SYN_ELSE) 
		columnType = NONE;
	else
		return -1;
	scaner();
	if (syn)//ȷ���������
		return -1;
	if (alterColumn(tableName,columnName,columnType))
		return -1;

	return 0;	

}
//�޸���
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

//���������
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

	scaner();
	if (syn)//ȷ���������
		return -1;
	if (truncateTable(tableName))
		return -1;
	return 0;
}
//ָ�����ݿ�
int useCmd()
{
	char databaseName[NAME_LENGHT];
	scaner();
	if (syn!=SYN_IDENTIFIER||isKeywords(word))
		return -1;
	if (!checkName(word)) //����Ӣ���ַ����»���
		return -1;
	strcpy(databaseName, word);

	scaner();
	if (syn)//ȷ���������
		return -1;
	if (use(databaseName))
		return -1;
	return 0;
}
//ɾ��������ݿ�
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
	if (syn==SYN_ELSE)
		return drop(databaseName, NULL);
	if (syn==SYN_IDENTIFIER){
		if (!checkName(word)) //����Ӣ���ַ����»���
			return -1;
		strcpy(tableName, word);
	}
	else
		return -1;

	scaner();
	if (syn)//ȷ���������
		return -1;
	if(drop(databaseName,tableName)) 
			return -1;
	return 0;
}
//���������ݿ�
int renameDatabaseCmd()
{//renameDatabase(char *oldName, char *newName);
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
	
	scaner();
	if (syn)//ȷ���������
		return -1;
	if (renameDatabase(oldName, newName))
		return -1;
	return 0;
}
//��������
int renameTableCmd()
{//renameTable(char *oldName, char *newName);
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
	
	scaner();
	if (syn)//ȷ���������
		return -1;
	if (renameTable(oldName, newName))
		return -1;
	return 0;
}
//����������
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
//ȷ�����������
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
//��ʾ���ݿ�
int showDatabaseCmd()
{//showDatabase(SORT_ORDER sortOrder);
	SORT_ORDER sortOrder;

	if (checkSort(&sortOrder))
		return -1;		
	
	scaner();
	if (syn)//ȷ���������
		return -1;
	if(showDatabase(sortOrder))
		return -1;
	return 0;
}
//��ʾ���ݱ��
int showTableCmd(char *databaseName)
{//showTable(char *databaseName,SORT_ORDER sortOrder);  //databaseNameΪNULLʱָ��Ϊ��ǰ���ݿ�
	SORT_ORDER sortOrder;

	if (checkSort(&sortOrder))
		return -1;

	scaner();
	if (syn)//ȷ���������
		return -1;
	if(showTable(databaseName, sortOrder))
		return -1;
	return 0;
}
//��ʾ����
int showColumnCmd(char *tableName)
{//showColumn(char *tableName, SORT_ORDER sortOrder);
	SORT_ORDER sortOrder;
	
	if (checkSort(&sortOrder))
		return -1;
	
	scaner();
	if (syn)//ȷ���������
		return -1;
	if(showColumn(tableName, sortOrder))
		return -1;
	return 0;
}
//������ʾ
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
//Ϊvalue��ֵ
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
	} else if (syn == SYN_PAREN_LEFT)//(
	{
		scaner();
		if (syn==SYN_SELECT)//select
		{
			num = 1; //(����Ŀ
			for (i=0;num>0;i++)
			{
				innerSelect[i]=p[i];
				if (p[i]=='(')
					num++;
				if (p[i]==')')
					num--;
			}
			
			innerSelect[i]='\0';
			temp = p+i;
			p = innerSelect;
			if (selectCmd(1, value))//�ڲ���ѯ
				return -1;
			p = temp;
		} else if (syn==SYN_INTEGER_NUMBER || syn== SYN_FLOAT_NUMBER || 
			syn == SYN_PAREN_LEFT || syn == SYN_MIMUS || syn== SYN_ADD)//int,float,(,-,+
		{
			num = 0; //(����Ŀ
			i=0;
			p=temp;
			do{
				innerSelect[i]=p[i];
				if (p[i]=='(')
					num++;
				if (p[i]==')')
					num--;
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
//ɾ������
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
	scaner();
	if (syn)//ȷ���������
		return -1;
	if (delete(tableName, colum, value))
		return -1;
	if (value.columnType==TEXT)
		free(value.columnValue.textValue);

	return 0;
}
//����update����
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
	if (!checkName(word)) //����Ӣ���ַ����»���
		return -1;
	strcpy(tableName, word);

	scaner();
	if (syn!=SYN_SET)//set
		return -1;

	scaner();
	if (syn==SYN_IDENTIFIER)
	{
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
	if (!checkName(word)) //����Ӣ���ַ����»���
		return -1;
	strcpy(updatedColumn, word);

	scaner();
	if (syn!=SYN_EQUAL)//==
		return -1;
	if (getValue(&oldValue))
		return -1;
	
	scaner();
	if (syn)//ȷ���������
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
//����insert����
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
	if (syn)//ȷ���������
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
//����Order�����������
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
//���һ��where������
int setOneWhere(Condition *condition)
{
	scaner();
	if (syn!=SYN_IDENTIFIER)//columnName
		return -1;
	if (!checkName(word))
		return -1;
	strcpy(condition->columnName, word);

	scaner();
	switch(syn)
	{
	case SYN_EQUAL://==
		condition->operator = EQ;
		//fall through
	case SYN_NOT_EQUAL://~=
		condition->operator = NE;
		//fall through
	case SYN_GREATER://>
		condition->operator = GT;
		//fall through
	case SYN_LESS://<
		condition->operator = LT;
		//fall through
	case SYN_MORE_EQUAL://>=
		condition->operator = GET;
		//fall through
	case SYN_LESS_EQUAL://<=
		condition->operator = LET;
		//getvalue
		if (getValue(&(condition->value)))
			return -1;
		break;
	case SYN_LIKE://like
		condition->operator = LIKE;
		if(getValue(&(condition->value)))
			return -1;
		break;
	case SYN_BETWEEN://between
		condition->operator = BETWEEN;
		scaner();
		if (syn!=SYN_BRACKET_LEFT)//[
			return -1;
		if(getValue(&(condition->value)))//value1
			return -1;
		scaner();
		if (syn!=SYN_COMMA)//,
			return -1;
		if (getValue(&(condition->value2)))//value2
			return -1;
		scaner();
		if (syn!=SYN_BRACKET_RIGHT)//]
			return -1;
		break;
	default:
		return -1;
		break;
	}
	return 0;
}
//����Ƿ���or,and
int checkLogic()
{
	int i;
	for (i=0;p[i];i++){//check and, or
		if (p[i]=='o'||p[i]=='O')
			if (p[i+1]=='r'||p[i+1]=='R')
				return 1;
		if (p[i]=='a'||p[i]=='A')
			if (p[i+1]=='n'||p[i+1]=='N')
				if (p[i+2]=='d'||p[i+2]=='D')
					return 1;
	}
	return 0;//û��
}
//����select����
int selectCmd(int isInner, Value *resultValue)
{//int select(SelectBody *selectBody);
	SelectBody selectBody;
	char columnsName[COL_NUM][NAME_LENGHT];
	char *colums[COL_NUM];
	char tableName[NAME_LENGHT];	
	int columnAmount, i;
	
	char sortColumnName[NAME_LENGHT];
	SORT_ORDER sortOrder;
	Condition conditon;

	scaner();
	if (syn==SYN_MULT) //*
	{
		selectBody.columnsName = NULL;
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
		if (checkLogic()){
			//����logic����;
			;
		} else {
			if (setOneWhere(&conditon))
				return -1;
		}
	} else if (syn == SYN_ORDER)//order
	{
		if (setSort(&sortOrder, sortColumnName))
			return -1;
	} 

	if (!isInner)//���
	{
		scaner();
		if (syn==SYN_ORDER){
			if (setSort(&sortOrder, sortColumnName))
				return -1;
			scaner();
		}
		if (syn!=SYN_ELSE)//end
			return -1;
	}
	//todo:select
	selectBody.tableName = tableName;
	selectBody.sortColumnName = sortColumnName;
	selectBody.sortOrder = sortOrder;	
	selectBody.isInner = isInner;
	selectBody.resultValue = resultValue;
	selectBody.condition = &conditon;

	if (select(&selectBody))
		return -1;

	return 0;
}
/* ����һ������cmd */
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
			flag = selectCmd(0, NULL);//���
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
