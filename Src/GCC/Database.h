 /**
 * @file    Database.h
 * @author  hzhigeng <hzhigeng@gmail.com>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * ���ݿ�ײ�����ṹ��Database�ṹ��Ϊ��һ���ṹ��Table�ṹ��Ϊ�ڶ�����ColumnΪ��������ColumnValueΪ���ļ���
 * ��������洢��Data��ͬ����
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
    Data data;                          //���ݴ洢��
    int hasData;                        //��ʶ�ý���Ƿ�������
    struct columnValue *next;
} ColumnValue;
typedef struct column {
    char columnName[NAME_MAX];          //����
    COLUMN_TYPE columnType;             //�е���������
    struct column *next;                //Column�����nextָ��
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
* ���ݲ���tableName������Ӧ��Table��������ָ�룬û���ҵ�����NULL
*/
Table *searchTable(char *tableName);

/**
* ����columnName����table���¶�Ӧ��Column��������ָ�룬�޷��ҵ��򷵻�NULL�����⣬prior�����洢
* �Ÿ�Column��ǰһ��Columnָ�룬�粻��Ҫ�ɽ�����ΪNULL��
*/
Column *searchColumn(Table *table, char *columnName, Column **prior);

/**
* ���ݲ���databaseName������Ӧ��Database��������ָ�룬û���ҵ�����NULL
*/
Database *searchDatabase(char *databaseName, Database **prior);

/**
* ����table�������е�Column������洢���������Ϊsize��allColumn���������Column����Ŀ
*/
int getAllColumn(Table *table, Column **allColumn, int size);

/**
* ���Դ�Сд�Ƚ��ַ���str1,str2�Ƿ�һ������ͬ����0
*/
int mystrcmp(const char *str1, const char *str2);

#endif // DATABASE_H_INCLUDED
