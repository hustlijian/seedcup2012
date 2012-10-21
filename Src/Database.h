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

Table *searchTable(char *tableName);
Column *searchColumn(Table *table, char *columnName, Column **prior);
#endif // DATABASE_H_INCLUDED
