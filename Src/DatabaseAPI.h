#ifndef DATABASEAPI_H_INCLUDED
#define DATABASEAPI_H_INCLUDED

typedef enum {INT, FLOAT, TEXT, NONE, EMPTY} COLUMN_TYPE;
typedef enum {EQ, NE, GT, LT, GET, LET, BETWEEN, LIKE} OPERATOR;
typedef enum {NOTSORT, DESC, INCR} SORT_ORDER;

typedef struct {
	COLUMN_TYPE columnType;
	union {
		int intValue;
		float floatValue;
		char *textValue;
	} columnValue;
} Value;
typedef struct condition {
	char *columnName;
	Value value;
	Value value2;
	OPERATOR operator;
	int logic;
	struct condition *nextCondition;
} Condition;
typedef struct {
	char *tableName;
	char **columnsName;
	int columnAmount;
	Condition *condition;
	SORT_ORDER sortOrder;
} SelectBody;
typedef struct {
	char *tableName;
	char **columnsName;
	Value *newValues;
	int columnAmount;
	char *updatedColumn;
	Value oldValue;
} UpdateBody;

/*�ڷ���ֵΪint�ĺ����У�����-1����ʧ�ܣ�����0����ɹ�*/
int createDatabase(char *databaseName);
int createTable(char *tableName, char **columnsName, COLUMN_TYPE *columnType, int columnAmount);
int addColumn(char *tableName, char *columnName, COLUMN_TYPE columnType);
int rmColumn(char *tableName, char *columnName);
int alterColumn(char *tableName, char *columnName, COLUMN_TYPE newColumnType);
int truncateTable(char *tableName);
int use(char *databaseName);
int drop(char *databaseName, char *tableName); 	//tableNameΪNULLʱɾ������database
int renameDatabase(char *oldName, char *newName);
int renameTable(char *oldName, char *newName);

//select�����ӿڿ�����Ҫ��д������Select���ܲ����������ݣ�ÿ�����ݼ���һ��Value*���飬
//�����ҿ��Ƿ���Value*�͵Ķ�ά���飬����rowAmountָ��ָ���ֵ��¼������
int select(SelectBody *selectBody, int *rowAmount);
int update(UpdateBody *updateBOdy);
//int delete(char *tableName, Condition *condition);
int delete(char *tableName, char *column, Value value);
int insert(char *tableName, char **columnsName, Value *values, int amount);

int showDatabase(SORT_ORDER sortOrder);
int showTable(char *databaseName,SORT_ORDER sortOrder);  //databaseNameΪNULLʱָ��Ϊ��ǰ���ݿ�
int showColumn(char *tableName, SORT_ORDER sortOrder);
int showColumnValue(char *tableName);

#endif // DATABASEAPI_H_INCLUDED
