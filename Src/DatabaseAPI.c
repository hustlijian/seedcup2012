#include "DatabaseAPI.h"

/*在返回值为int的函数中，返回-1代表失败，返回0代表成功*/
int createDatabase(char *databaseName);
int createTable(char *tableName, char **columnsName, COLUMN_TYPE *columnType, int columnAmount);
int addColumn(char *tableName, char *columnName, COLUMN_TYPE columnType);
int rmColumn(char *tableName, char *columnName);
int alterColumn(char *tableName, char *columnName, COLUMN_TYPE newColumnType);
int truncateTable(char *tableName);
int use(char *databaseName);
int drop(char *databaseName, char *tableName); 	//tableName为NULL时删除整个database
int rename(char *oldName, char newName, int renameTable); //1为重命名表，0为重命名数据库

//select函数接口可能需要重写，由于Select可能产生多行数据，每行数据即是一个Value*数组，
//所以我考虑返回Value*型的二维数组，其中rowAmount指针指向的值记录了行数
Value*** select(SelectBody *selectBody, int *rowAmount);
int update(UpdateBody *updateBOdy);
//int delete(char *tableName, Condition *condition);
int delete(char *tableName, char *column, Value value);
int insert(char *tableName, char **columnsName, Value *values, int amount);

int showDatabase(char *databaseName, SORT_ORDER sortOrder);
int showTable(char *databaseName, char *tableName, SORT_ORDER sortOrder);  //databaseName为NULL时指定为当前数据库
int showColumn(char *tableName, SORT_ORDER sortOrder);

