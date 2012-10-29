#ifndef DATABASEAPI_H_INCLUDED
#define DATABASEAPI_H_INCLUDED

#define NAME_MAX 25

/**
* 数据类型，其中EMPTY意味着该数据结点并不包含数据
*/
typedef enum {INT, FLOAT, TEXT, NONE, EMPTY} COLUMN_TYPE;

/**
* 操作符：==, ~=, >, <, >=, <= ,BETWEEN, LIKE
*/
typedef enum {EQ, NE, GT, LT, GET, LET, BETWEEN, LIKE} OPERATOR;

/**
* 排序方式：不排序，降序，升序
*/
typedef enum {NOTSORT, DESC, INCR} SORT_ORDER;

/**
* 逻辑操作符：NOLOGIC主要用于标识不包含逻辑关系的结点及逻辑链表的尾结点
*/
typedef enum {OR, AND, NOLOGIC} LOGIC;

typedef struct {
	COLUMN_TYPE columnType;         //数据类型
	union {
		int intValue;               //INT数据
		float floatValue;           //FLOAT数据
		char *textValue;            //TEXT数据
	} columnValue;
} Value;
typedef struct condition {
	char columnName[NAME_MAX];      //where后每个条件中的列名
	Value value;                    //第一个数据
	Value value2;                   //第二个数据，主要用于BETWEEN操作符
	OPERATOR operator;              //操作符
	LOGIC logic;                    //逻辑关系
	struct condition *next;         //Condition链表的next指针
} Condition;
typedef struct {
	char *tableName;                //select操作的表的名字
	char **columnsName;             //select操作的列的名字，NULL代表表中的所有列
	int columnAmount;               //select操作的列的数目
	Value *resultValue;             //当select为嵌套内层时，此处存储着处理后的结果
	int isInner;                    //标识是否为嵌套内层，1代表是
	char *sortColumnName;           //排序所根据的列的列名
	SORT_ORDER sortOrder;           //排序方式
	Condition *condition;           //Condition链表，存储着所有where的条件
} SelectBody;                       //存储着select功能所需的所有参数
typedef struct {
	char *tableName;                //update操作的表的名字
	char **columnsName;             //update操作中所有需要更新的列的名字
	Value *newValues;               //update操作中更新后列的数据
	int columnAmount;               //需要更新的列的数目
	char *updatedColumn;            //update中where条件的列名
	Value oldValue;                 //update中where条件的数据
} UpdateBody;                       //存储着update功能所需的所有参数

/*在返回值为int的函数中，返回-1代表失败，返回0代表成功*/
int createDatabase(char *databaseName);
int createTable(char *tableName, char **columnsName, COLUMN_TYPE *columnType, int columnAmount);
int addColumn(char *tableName, char *columnName, COLUMN_TYPE columnType);
int rmColumn(char *tableName, char *columnName);
int alterColumn(char *tableName, char *columnName, COLUMN_TYPE newColumnType);
int truncateTable(char *tableName);
int use(char *databaseName);
int drop(char *databaseName, char *tableName); 	//tableName为NULL时删除整个database
int renameDatabase(char *oldName, char *newName);
int renameTable(char *oldName, char *newName);

int select(SelectBody *selectBody);
int update(UpdateBody *updateBOdy);
int delete(char *tableName, char *column, Value value);
int insert(char *tableName, char **columnsName, Value *values, int amount);

int showDatabase(SORT_ORDER sortOrder);
int showTable(char *databaseName,SORT_ORDER sortOrder);  //databaseName为NULL时指定为当前数据库
int showColumn(char *tableName, SORT_ORDER sortOrder);
int showColumnValue(char *tableName);

#endif // DATABASEAPI_H_INCLUDED
