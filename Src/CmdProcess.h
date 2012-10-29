/**
 * @file	CmdProcess.h
 * @author  lijian <hustlijian@gmail.com>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * ���������ͷ�ļ��������˴�ɨ���룬��������
 */


#ifndef CMDPROCESS_H
#define CMDPROCESS_H

#include "DatabaseAPI.h"
#include "Tree.h"

#define NUM_KEYWORDS 33
#define MAX_KEYWORDS_LENGTH 10
#define NUM_OPERATOR	20
#define MAX_OPERATOR	3
#define NUM_SPACE		3
#define MAX_WORDLEGTH  1024

#define SYN_CREATE		1		//create
#define SYN_DATABASE	2		//database
#define SYN_TABLE		3		//table
#define SYN_ALTER		4		//alter
#define SYN_TRUNCATE	5		//truncate
#define SYN_ADD			6		//add
#define SYN_COLUMN		7		//column
#define SYN_USE			8		//use
#define SYN_DROP		9		//drop
#define SYN_RENAME		10		//rename
#define SYN_SELECT		11		//select
#define SYN_FROM		12		//from
#define SYN_WHERE		13		//where
#define SYN_ORDER		14		//order
#define SYN_BY			15		//by
#define SYN_DESC		16		//desc
#define SYN_INCR		17		//incr
#define SYN_UPDATE		18		//update
#define SYN_SET			19		//set
#define SYN_DELETE		20		//delete
#define SYN_INSERT		21		//insert
#define SYN_INTO		22		//into
#define SYN_VALUES		23		//values
#define SYN_SHOW		24		//show
#define SYN_DATABASES	25		//databases
#define SYN_INT			26		//int
#define SYN_FLOAT		27		//float
#define SYN_TEXT		28		//text
#define SYN_NONE		29		//none
#define SYN_BETWEEN		30		//between
#define SYN_LIKE		31		//like
#define SYN_AND			32		//and
#define SYN_OR			33		//or

#define SYN_IDENTIFIER	40		//identifier
#define SYN_INTEGER_NUMBER	41	//integer number
#define SYN_FLOAT_NUMBER	42	//float number

#define SYN_OPERATOR_BASE	50
#define SYN_PAREN_LEFT	50		//(
#define SYN_PAREN_RIGHT	51		//)
#define SYN_MULT		52		//*
#define SYN_DIVI		53		///
#define SYN_MOD			54		//%
#define SYN_PULSE		55		//+
#define SYN_MIMUS		56		//-
#define SYN_EQUAL		57		//==
#define SYN_NOT_EQUAL	58		//~=
#define SYN_MORE_EQUAL	59		//>=
#define SYN_LESS_EQUAL	60		//<=
#define SYN_GREATER		61		//>
#define SYN_LESS		62		//<
#define SYN_BRACKET_LEFT	63	//[
#define SYN_BRACKET_RIGHT	64	//]
#define SYN_SEMICOLON	65		//;
#define SYN_COMMA		66		//,
#define SYN_QUOTE		67		//'
#define SYN_QUESTION	68		//?
#define SYN_ASSIGN		69		//=
#define SYN_ELSE		0


/*�ڷ���ֵΪint�ĺ����У�����-1����ʧ�ܣ�����0����ɹ�*/
//�����ִ�Сд�ַ����Ƚ�
int mystrcmp(const char *str1, const char *str2);
//��ȡ�ַ����дʵ�����
int getTypeNum(char *str);
//��ȡֵ����
COLUMN_TYPE getValueType(char *str);
//��ȡ����������
OPERATOR getOptType(char *str);
//�ж��Ƿ��ǲ�����
int isLgcExpOpt(char *str);
//��ȡ�߼�����
LOGIC getLogicType(char *str);
//ÿ��ɨ����һ���ʣ��������ֱ���
void scaner();
//�ж��Ƿ��ǹؼ���
int isKeywords(char *str);
//�ж��Ƿ���������Ҫ������ĸ�»������
int processCmd(char *cmd);
//�������ݿ������
int createDatabaseCmd();
//�����������
int createTableCmd();
//�������ݿ���߱�
int createCmd();
//�����
int alterAddCmd(char *tableName);
//ɾ����
int alterRmCmd(char *tableName);
//�޸�������
int alterAlterCmd(char *tableName);
//�޸���
int alterCmd();
//���������
int truncateCmd();
//ָ�����ݿ�
int useCmd();
//ɾ��������ݿ�
int dropCmd();
//���������ݿ�
int renameDatabaseCmd();
//��������
int renameTableCmd();
//����������
int renameCmd();
//ȷ�����������
int  checkSort(SORT_ORDER *sortOrder);
//��ʾ���ݿ�
int showDatabaseCmd();
//��ʾ���ݱ��
int showTableCmd(char *databaseName);
//��ʾ����
int showColumnCmd(char *tableName);
//������ʾ
int showCmd();
//Ϊvalue��ֵ
int getValue(Value *value);
//ɾ������
int deleteCmd();
//����update����
int updateCmd();
//����insert����
int insertCmd();
//����Order�����������
int setSort(SORT_ORDER *sortOrder, char *sortColumn);
//���һ��where������
int setOneWhere(Condition *condition);
//����Ƿ���or,and
int checkLogic();

//����select����
int selectCmd(int isInner, Value *resultValue);
/* ����һ������cmd */
int processCmd(char *cmd);

//LOGIC
int expression(Node *T);
int term(Node *T);
int factor(Node *T);
//����and, or���ʽ����Ϊ����ȥ������
int setLogicWhere(char *str, Condition **conditon);

#endif //CMDPROCESS_H