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

#define SYN_CREATE		1
#define SYN_DATABASE	2
#define SYN_TABLE		3
#define SYN_ALTER		4
#define SYN_TRUNCATE	5
#define SYN_ADD			6
#define SYN_COLUMN		7
#define SYN_USE			8
#define SYN_DROP		9
#define SYN_RENAME		10
#define SYN_SELECT		11
#define SYN_FROM		12
#define SYN_WHERE		13
#define SYN_ORDER		14
#define SYN_BY			15
#define SYN_DESC		16
#define SYN_INCR		17
#define SYN_UPDATE		18
#define SYN_SET			19
#define SYN_DELETE		20
#define SYN_INSERT		21
#define SYN_INTO		22
#define SYN_VALUES		23
#define SYN_SHOW		24
#define SYN_DATABASES	25
#define SYN_INT			26
#define SYN_FLOAT		27
#define SYN_TEXT		28
#define SYN_NONE		29
#define SYN_BETWEEN		30
#define SYN_LIKE		31
#define SYN_AND			32
#define SYN_OR			33

#define SYN_IDENTIFIER	40
#define SYN_INTEGER_NUMBER	41
#define SYN_FLOAT_NUMBER	42

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
static void scaner();
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
/*
 *�����߼�����
 *����ǰ�뱣��ȫ��ָ��P�����ú�ȫ��ָ��ָ����ʽ֮��
 */
int logicalExpProc(char *expStr, Condition **expList);
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