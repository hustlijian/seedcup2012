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


/*在返回值为int的函数中，返回-1代表失败，返回0代表成功*/
//不区分大小写字符串比较
int mystrcmp(const char *str1, const char *str2);
//获取字符串中词的类型
int getTypeNum(char *str);
//获取值类型
COLUMN_TYPE getValueType(char *str);
//获取操作符类型
OPERATOR getOptType(char *str);
//判断是否是操作符
int isLgcExpOpt(char *str);
//获取逻辑类型
LOGIC getLogicType(char *str);
//每次扫描获得一个词，和他的种别码
static void scaner();
//判断是否是关键字
int isKeywords(char *str);
//判断是否满足名字要求，又字母下划线组成
int processCmd(char *cmd);
//建立数据库的命令
int createDatabaseCmd();
//建立表的命令
int createTableCmd();
//建立数据库或者表
int createCmd();
//添加列
int alterAddCmd(char *tableName);
//删除列
int alterRmCmd(char *tableName);
//修改列类型
int alterAlterCmd(char *tableName);
//修改列
int alterCmd();
//清除表数据
int truncateCmd();
//指定数据库
int useCmd();
//删除表或数据库
int dropCmd();
//重命名数据库
int renameDatabaseCmd();
//重命名表
int renameTableCmd();
//处理重命名
int renameCmd();
//确认排序的类型
int  checkSort(SORT_ORDER *sortOrder);
//显示数据库
int showDatabaseCmd();
//显示数据表格
int showTableCmd(char *databaseName);
//显示列名
int showColumnCmd(char *tableName);
//处理显示
int showCmd();
//为value赋值
int getValue(Value *value);
//删除命令
int deleteCmd();
//处理update命令
int updateCmd();
//处理insert命令
int insertCmd();
//有了Order命令，设置类型
int setSort(SORT_ORDER *sortOrder, char *sortColumn);
//获得一个where的条件
int setOneWhere(Condition *condition);
//检查是否有or,and
int checkLogic();
/*
 *复杂逻辑运算
 *调用前须保存全局指针P，调用后将全局指针指向表达式之后
 */
int logicalExpProc(char *expStr, Condition **expList);
//处理select命令
int selectCmd(int isInner, Value *resultValue);
/* 处理一条命令cmd */
int processCmd(char *cmd);

//LOGIC
int expression(Node *T);
int term(Node *T);
int factor(Node *T);
//复杂and, or表达式，化为链，去除括号
int setLogicWhere(char *str, Condition **conditon);

#endif //CMDPROCESS_H