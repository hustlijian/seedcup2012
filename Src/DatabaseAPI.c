#include "DatabaseAPI.h"

/*�ڷ���ֵΪint�ĺ����У�����-1����ʧ�ܣ�����0����ɹ�*/
int createDatabase(char *databaseName);
int createTable(char *tableName, char **columnsName, COLUMN_TYPE *columnType, int columnAmount);
int addColumn(char *tableName, char *columnName, COLUMN_TYPE columnType);
int rmColumn(char *tableName, char *columnName);
int alterColumn(char *tableName, char *columnName, COLUMN_TYPE newColumnType);
int truncateTable(char *tableName);
int use(char *databaseName);
int drop(char *databaseName, char *tableName); 	//tableNameΪNULLʱɾ������database
int rename(char *oldName, char newName, int renameTable); //1Ϊ��������0Ϊ���������ݿ�

//select�����ӿڿ�����Ҫ��д������Select���ܲ����������ݣ�ÿ�����ݼ���һ��Value*���飬
//�����ҿ��Ƿ���Value*�͵Ķ�ά���飬����rowAmountָ��ָ���ֵ��¼������
Value*** select(SelectBody *selectBody, int *rowAmount);
int update(UpdateBody *updateBOdy);
//int delete(char *tableName, Condition *condition);
int delete(char *tableName, char *column, Value value);
int insert(char *tableName, char **columnsName, Value *values, int amount);

int showDatabase(char *databaseName, SORT_ORDER sortOrder);
int showTable(char *databaseName, char *tableName, SORT_ORDER sortOrder);  //databaseNameΪNULLʱָ��Ϊ��ǰ���ݿ�
int showColumn(char *tableName, SORT_ORDER sortOrder);

