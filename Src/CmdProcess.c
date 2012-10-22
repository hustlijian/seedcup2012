#include <stdio.h>
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
	} while ((c1==c2)&&c1);
	return (c1-c2);
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
		if (flag==1) return 42; //������
		if (flag>1) return 0;//��������
		return 41; //����
	} 
	if (isalpha(str[0])||str[0]=='_') //��ʶ��
	{
		for (i=0;i<NUM_KEYWORDS;i++)
			if (!mystrcmp(keywords[i], str))
				break;
		if (i==NUM_KEYWORDS) return 40;//һ���ʶ��
		else return (i+1);
	}
	//�����
	for (i=0;i<NUM_OPERATOR;i++)
		if (!strcmp(str, operatorwords[i]))
			break;
	if (i<NUM_OPERATOR) return (i+50);
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
		syn = 0;
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
	return 0;  //���ǹؼ���
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
	if (syn!=40||isKeywords(word)) //���ǹؼ���
		return -1;
	if (!checkDatabaseName(word)) //�����������Ӣ���ַ����»���
		return -1;
	strcpy(databaseName, word);

	scaner();
	if (syn)//ȷ���������
		return -1;
	if (createDatabase(word))//�������ݿ�
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
	if (syn!=40 || isKeywords(word))//����ʹ�ùؼ���
		return -1;
	if (!checkDatabaseName(word)) //�����������Ӣ���ַ����»���
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
		strcpy(columnsName[countAmount],word);

		scaner();
		if(syn>25&&syn<29)  //ָ������26,27,28
			columnType[countAmount] = syn-26;
		else {
			columnType[countAmount] = NONE;
			if (syn==51) break; //������
			if (syn!=66) return -1; //����
		}

		scaner();
		if (syn==51) break; //������
		if (syn!=66) return -1; //����
	}

	for (i=0;i<=countAmount;i++)
	{
		p[i]=columnsName[i];
	}
	
	scaner();
	if (syn)//ȷ���������
		return -1;

	if(createTable(tableName,p, columnType, countAmount+1))
		return -1;
	return 0;
}
//�������ݿ���߱�
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
//������
int alterAddCmd(char *tableName)
{
	char columnName[NAME_LENGHT];
	COLUMN_TYPE columnType;

	scaner();
	if (syn!=40)
		return -1;
	strcpy(columnName,word);

	scaner();
	if(syn>25&&syn<29)  //ָ������26,27,28
		columnType = syn-26;
	else if (syn==0) 
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
	if (syn!=7)
		return -1;

	scaner();
	if (syn!=40||isKeywords(word))
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
	if (syn!=7)
		return -1;

	scaner();
	if (syn!=40)
		return -1;
	strcpy(columnName,word);
	
	scaner();
	if(syn>25&&syn<29)  //ָ������26,27,28
		columnType = syn-26;
	else if (syn==0) 
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

//���������
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
	if (syn!=40||isKeywords(word))
		return -1;
	strcpy(databaseName, word);

	scaner();
	if (syn)//ȷ���������
		return -1;
	if (use(databaseName))
		return -1;
	return 0;
}
//ɾ���������ݿ�
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
	if (syn!=40||isKeywords(word))
		return -1;
	strcpy(oldName, word);

	scaner();
	if (syn!=40||isKeywords(word))
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
	if (syn!=40||isKeywords(word))
		return -1;
	strcpy(oldName, word);
	
	scaner();
	if (syn!=40||isKeywords(word))
		return -1;
	strcpy(newName, word);
	
	scaner();
	if (syn)//ȷ���������
		return -1;
	if (renameTable(oldName, word))
		return -1;
	return 0;
}
//����������
int renameCmd()
{
	scaner();
	if (syn==2)//���ݿ�
	{
		return renameDatabaseCmd();
	} else if(syn==3)//��
	{
		return renameTableCmd();
	}
	else return -1;
}
//��ʾ���ݿ�
int showDatabaseCmd()
{//showDatabase(SORT_ORDER sortOrder);
	SORT_ORDER sortOrder;

	scaner();
	if (syn==0) //NOSORT
		sortOrder = NOTSORT;
	else if (syn==14)//SORT
	{
		scaner();
		if (syn!=15)//by
			return -1;
		scaner();
		if (syn==16)
			sortOrder = DESC;
		else if (syn==17)
			sortOrder = INCR;
		else 
			return -1;
	} else
		return -1;		
	
	scaner();
	if (syn)//ȷ���������
		return -1;
	showDatabase(sortOrder);//no error will appear
	return 0;
}
//��ʾ���ݱ���
int showTableCmd(char *databaseName)
{//showTable(char *databaseName,SORT_ORDER sortOrder);  //databaseNameΪNULLʱָ��Ϊ��ǰ���ݿ�
	SORT_ORDER sortOrder;

	scaner();
	if (syn==0) //NOSORT
		sortOrder = NOTSORT;
	else if (syn==14)//SORT
	{
		scaner();
		if (syn!=15)//by
			return -1;
		scaner();
		if (syn==16)
			sortOrder = DESC;
		else if (syn==17)
			sortOrder = INCR;
		else 
			return -1;
	} else
		return -1;

	scaner();
	if (syn)//ȷ���������
		return -1;
	showTable(databaseName, sortOrder);//no error whill appear
	return 0;
}
//��ʾ����
int showColumnCmd(char *tableName)
{//showColumn(char *tableName, SORT_ORDER sortOrder);
	SORT_ORDER sortOrder;
	
	scaner();
	if (syn==0) //NOSORT
		sortOrder = NOTSORT;
	else if (syn==14)//SORT
	{
		scaner();
		if (syn!=15)//by
			return -1;
		scaner();
		if (syn==16)
			sortOrder = DESC;
		else if (syn==17)
			sortOrder = INCR;
		else 
			return -1;
	} else
		return -1;
	
	scaner();
	if (syn)//ȷ���������
		return -1;
	showColumn(tableName, sortOrder);//no error will appear
	return 0;
}
//������ʾ
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
//ɾ������
int deleteCmd()
{//delete(char *tableName, char *column, Value value);
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
		default: 
			break;
		}

	}
	return flag;
}
/*�������Եĳ���*/
void test()
{
	freopen("input.txt", "r", stdin); 
	freopen("out.txt","w", stdout);
}