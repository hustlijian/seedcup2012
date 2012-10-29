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
        int intValue;					//INT����
        float floatValue;				//FLOAT����
        char *textValue;				//TEXT����
} Data;
typedef struct columnValue {
    Data data;                          //���ݴ洢��
    int hasData;                        //��ʶ�ý���Ƿ�������
    struct columnValue *next;           //ColumnValue�����nextָ��
} ColumnValue;
typedef struct column {
    char columnName[NAME_MAX];          //����
    COLUMN_TYPE columnType;             //�е���������
    struct column *next;                //Column�����nextָ��
    ColumnValue *columnValueHead;       //ColumnValue�����ͷָ��
} Column;
typedef struct table {
    char tableName[NAME_MAX];           //����
    struct table *next;                 //Table�����nextָ��
    Column *columnHead;                 //Column�����ͷָ��
} Table;
typedef struct database {
    char databaseName[NAME_MAX];        //���ݿ���
    struct database *next;              //Database�����nextָ��
    Table *tableHead;                   //Table�����ͷָ��
} Database;

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

#endif // DATABASE_H_INCLUDED
