#ifndef DATABASEAPI_H_INCLUDED
#define DATABASEAPI_H_INCLUDED

#define NAME_MAX 25

/**
* �������ͣ�����EMPTY��ζ�Ÿ����ݽ�㲢����������
*/
typedef enum {INT, FLOAT, TEXT, NONE, EMPTY} COLUMN_TYPE;

/**
* ��������==, ~=, >, <, >=, <= ,BETWEEN, LIKE
*/
typedef enum {EQ, NE, GT, LT, GET, LET, BETWEEN, LIKE} OPERATOR;

/**
* ����ʽ�������򣬽�������
*/
typedef enum {NOTSORT, DESC, INCR} SORT_ORDER;

/**
* �߼���������NOLOGIC��Ҫ���ڱ�ʶ�������߼���ϵ�Ľ�㼰�߼������β���
*/
typedef enum {OR, AND, NOLOGIC} LOGIC;

typedef struct {
	COLUMN_TYPE columnType;         //��������
	union {
		int intValue;               //INT����
		float floatValue;           //FLOAT����
		char *textValue;            //TEXT����
	} columnValue;
} Value;
typedef struct condition {
	char columnName[NAME_MAX];      //where��ÿ�������е�����
	Value value;                    //��һ������
	Value value2;                   //�ڶ������ݣ���Ҫ����BETWEEN������
	OPERATOR operator;              //������
	LOGIC logic;                    //�߼���ϵ
	struct condition *next;         //Condition�����nextָ��
} Condition;
typedef struct {
	char *tableName;                //select�����ı������
	char **columnsName;             //select�������е����֣�NULL������е�������
	int columnAmount;               //select�������е���Ŀ
	Value *resultValue;             //��selectΪǶ���ڲ�ʱ���˴��洢�Ŵ����Ľ��
	int isInner;                    //��ʶ�Ƿ�ΪǶ���ڲ㣬1������
	char *sortColumnName;           //���������ݵ��е�����
	SORT_ORDER sortOrder;           //����ʽ
	Condition *condition;           //Condition�����洢������where������
} SelectBody;                       //�洢��select������������в���
typedef struct {
	char *tableName;                //update�����ı������
	char **columnsName;             //update������������Ҫ���µ��е�����
	Value *newValues;               //update�����и��º��е�����
	int columnAmount;               //��Ҫ���µ��е���Ŀ
	char *updatedColumn;            //update��where����������
	Value oldValue;                 //update��where����������
} UpdateBody;                       //�洢��update������������в���

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

int select(SelectBody *selectBody);
int update(UpdateBody *updateBOdy);
int delete(char *tableName, char *column, Value value);
int insert(char *tableName, char **columnsName, Value *values, int amount);

int showDatabase(SORT_ORDER sortOrder);
int showTable(char *databaseName,SORT_ORDER sortOrder);  //databaseNameΪNULLʱָ��Ϊ��ǰ���ݿ�
int showColumn(char *tableName, SORT_ORDER sortOrder);
int showColumnValue(char *tableName);

#endif // DATABASEAPI_H_INCLUDED
