 /**
 * @file    Database.h
 * @author  hzhigeng <hzhigeng@gmail.com>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * 数据库底层物理结构，Database结构体为第一级结构，Table结构体为第二级，Column为第三级，ColumnValue为第四级，
 * 而数据则存储在Data共同体中
 */

#ifndef DATABASE_H_INCLUDED
#define DATABASE_H_INCLUDED

#include "DatabaseAPI.h"


typedef union {
        int intValue;
        float floatValue;
        char *textValue;
} Data;
typedef struct columnValue {
    Data data;                          //数据存储处
    int hasData;                        //标识该结点是否含有数据
    struct columnValue *next;
} ColumnValue;
typedef struct column {
    char columnName[NAME_MAX];          //列名
    COLUMN_TYPE columnType;             //列的数据类型
    struct column *next;                //Column链表的next指针
    ColumnValue *columnValueHead;
} Column;
typedef struct table {
    char tableName[NAME_MAX];
    struct table *next;
    Column *columnHead;
}Table;

typedef struct database {
    char databaseName[NAME_MAX];
    struct database *next;
    Table *tableHead;
}Database;

/**
* 根据参数tableName搜索对应的Table并返回其指针，没有找到返回NULL
*/
Table *searchTable(char *tableName);

/**
* 根据columnName搜索table底下对应的Column并返回其指针，无法找到则返回NULL，另外，prior参数存储
* 着该Column的前一个Column指针，如不需要可将其设为NULL，
*/
Column *searchColumn(Table *table, char *columnName, Column **prior);

/**
* 根据参数databaseName搜索对应的Database并返回其指针，没有找到返回NULL
*/
Database *searchDatabase(char *databaseName, Database **prior);

/**
* 查找table底下所有的Column并将其存储在最大容量为size的allColumn里，函数返回Column的数目
*/
int getAllColumn(Table *table, Column **allColumn, int size);

/**
* 忽略大小写比较字符串str1,str2是否一样，相同返回0
*/
int mystrcmp(const char *str1, const char *str2);

#endif // DATABASE_H_INCLUDED
