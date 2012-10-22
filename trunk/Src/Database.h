#ifndef DATABASE_H_INCLUDED
#define DATABASE_H_INCLUDED

#include "DatabaseAPI.h"

#define LENGTH 25

typedef union {
        int intValue;
        float floatValue;
        char *textValue;
} Data;
typedef struct columnValue {
    Data data;
    int hasData;
    struct columnValue *next;
} ColumnValue;
typedef struct column {
    char columnName[LENGTH];
    COLUMN_TYPE columnType;
    struct column *next;
    ColumnValue *columnValueHead;
} Column;
typedef struct table {
    char tableName[LENGTH];
    struct table *next;
    Column *columnHead;
} Table;
typedef struct database {
    char databaseName[LENGTH];
    struct database *next;
    Table *tableHead;
} Database;

//本来下面3个函数应该拥有相同的形式，但由于整体上没有考虑清楚，导致
//对函数功能定义不明确，所以就这样了……暂时不改了
Table *searchTable(char *tableName);
Column *searchColumn(Table *table, char *columnName, Column **prior);
Database *searchDatabase(char *databaseName, Database **prior);
int getAllColumn(Table *table, Column **allColumn, int size);

#endif // DATABASE_H_INCLUDED
