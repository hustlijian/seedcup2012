 /**
 * @file    ShowInfo.c
 * @author  hzhigeng <hzhigeng@gmail.com>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * ��ʾ���֣��������"SHOW DATABASES/TABLE/COLUMN������ع��ܡ�
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Database.h"

#define OUTPUT_MAX  20
#define SIZE   20

static int descOrderCompare(const void *elem1, const void *elem2);
static int incrOrderCompare(const void *elem1, const void *elem2);
static void sortStringArray(char **stringArray, int size,
                     SORT_ORDER sortOrder);
static void outputStringArray(char **stringArray, int size);
static void freeStringArray(char **stringArray, int size);
static char *enumToString(COLUMN_TYPE columnType);
static void outputValue(ColumnValue *columnValue, COLUMN_TYPE columnType);

extern Database *head;
extern Database *currentDatabase;

/**
 * <��ʾ���е����ݿ�����>
 *
 * @param   sortOrder     ����ʽ
 * @return  0��������ɹ�
 */
int showDatabase(SORT_ORDER sortOrder)
{
	Database *databaseTra = head;
    int count = 0;
    char *databasesName[OUTPUT_MAX];
    memset(databasesName, 0, OUTPUT_MAX*sizeof(char *));

    if (databaseTra == NULL)
    {
        printf("$\n");   //NO DATA
        return 0;
    }

    while (databaseTra != NULL)
    {
        databasesName[count] = calloc(NAME_MAX, sizeof(char));
        strncpy(databasesName[count++], databaseTra->databaseName, NAME_MAX-1);
        databaseTra = databaseTra->next;
    }
    sortStringArray(databasesName, count, sortOrder);
    outputStringArray(databasesName, count);
    freeStringArray(databasesName, count);
    return 0;
}
/**
 * <��ʾĳ�����ݿ�����б������>
 *
 * @param   databaseName     ���ݿ�����NULL����ָ��Ϊ��ǰ���ݿ�
 * @param   sortOrder        ����ʽ
 * @return  0��������ɹ���-1�������ݿⲻ����
 */
int showTable(char *databaseName, SORT_ORDER sortOrder)
{
    Database *database;
	char *tablesName[OUTPUT_MAX];
	Table *tableTra;
	int count = 0;

    if (databaseName == NULL)
        database = currentDatabase;
    else
        database = searchDatabase(databaseName, NULL);
    if (database == NULL)
        return -1;

    memset(tablesName, 0, OUTPUT_MAX*sizeof(char *));
    tableTra = database->tableHead;
    if (tableTra == NULL)
    {
        printf("$\n");
        return 0;
    }

    while (tableTra != NULL)
    {
        tablesName[count] = calloc(NAME_MAX, sizeof(char));
        strncpy(tablesName[count++], tableTra->tableName, NAME_MAX-1);
        tableTra = tableTra->next;
    }
    sortStringArray(tablesName, count, sortOrder);
    outputStringArray(tablesName, count);
    freeStringArray(tablesName, count);
    return 0;
}
/**
 * <��ʾĳһ����������е�����>
 *
 * @param   tableName   ����
 * @param   sortOrder   ����ʽ
 * @return  0��������ɹ���-1���������
 */
int showColumn(char *tableName, SORT_ORDER sortOrder)
{
	char *columnsName[OUTPUT_MAX];
	Column *columnTra;
	 int count = 0;
    Table *table = searchTable(tableName);
    if (table == NULL)
        return -1;

    memset(columnsName, 0, OUTPUT_MAX*sizeof(char *));
    columnTra = table->columnHead;
    if (columnTra == NULL)
    {
        printf("$\n");
        return 0;
    }

    while (columnTra != NULL)
    {
        columnsName[count] = calloc(NAME_MAX, sizeof(char));
        strncpy(columnsName[count++], columnTra->columnName, NAME_MAX-1);

        columnTra = columnTra->next;
    }
    sortStringArray(columnsName, count, sortOrder);
    outputStringArray(columnsName, count);
    freeStringArray(columnsName, count);
    return 0;
}
static int showColumnType(char *tableName)
{
	Column *columnTra;
    Table *table = searchTable(tableName);
    if (table == NULL)
        return -1;

    columnTra = table->columnHead;
    if (columnTra == NULL)
    {
        printf("$\n");
        return 0;
    }
    while (columnTra != NULL)
    {
        printf("%s, ", enumToString(columnTra->columnType));
        columnTra = columnTra->next;
    }
    putchar('\n');
    return 0;
}
/**
 * <��ʾĳһ����������е�����>
 *
 * <�˺�����Ҫ���ڵ���>
 *
 * @param   tableName   ����
 * @return  0��������ɹ���-1��������ڻ�ñ������κ���
 */
int showAllColumnValue(char *tableName)
{
	Column *allColumn[OUTPUT_MAX];
	int length;
	ColumnValue *columnValueTra[SIZE];
    int i;
    Table *table = searchTable(tableName);
    if (table == NULL)
        return -1;

    length = getAllColumn(table, allColumn, OUTPUT_MAX);
    if (length <= 0)
        return -1;

    printf("Begin -- \n");
    showColumn(tableName, NOTSORT);
    showColumnType(tableName);
    if (allColumn[0] == NULL)
        printf("$\n");
    else
    {
        for (i = 0; i < length; i++)
            columnValueTra[i] = allColumn[i]->columnValueHead;
        if (columnValueTra[0] == NULL)
            printf("$\n");
        while (columnValueTra[0] != NULL)
        {
            for (i = 0; i < length; i++)
            {
                outputValue(columnValueTra[i], allColumn[i]->columnType);
                columnValueTra[i] = columnValueTra[i]->next;
            }
            printf("\n");
        }
    }
    printf("End -- \n");
    return 0;
}
static void sortStringArray(char **stringArray, int size, SORT_ORDER sortOrder)
{
	int (*compare)(const void *, const void *);
    if (sortOrder == NOTSORT)
        return ;
    
    if (sortOrder == DESC)
        compare = descOrderCompare;
    else
        compare = incrOrderCompare;
    qsort(stringArray, size, sizeof(char *), compare);
}
static int descOrderCompare(const void *elem1, const void *elem2)
{
    return -strcmp(*(char **)elem1, *(char **)elem2);
}
static int incrOrderCompare(const void *elem1, const void *elem2)
{
    return strcmp(*(char **)elem1, *(char **)elem2);
}
static void outputStringArray(char **stringArray, int size)
{
    int i;
    char *searchBlank;

    for (i = 0; i < size; i++)
    {
        searchBlank = strchr(stringArray[i], ' ');
        if (searchBlank != NULL)
            printf("\"%s\"", stringArray[i]);
        else
            printf("%s", stringArray[i]);
        if (i < size - 1)
            putchar(',');
    }
    putchar('\n');
}
static void freeStringArray(char **stringArray, int size)
{
    int i;
    for (i = 0; i < size; i++)
        free(stringArray[i]);
}

static char *enumToString(COLUMN_TYPE columnType)
{
    switch (columnType)
    {
    case INT:
        return "INT";
    case FLOAT:
        return "FLOAT";
    case TEXT:
        return "TEXT";
    default:
        return "NONE";
    }
}
static void outputValue(ColumnValue *columnValue, COLUMN_TYPE columnType)
{
    Data data = columnValue->data;
    if (columnValue->hasData)
    {
        switch (columnType)
        {
        case INT:
            printf("%d,", data.intValue);
            break;
        case FLOAT:
            printf("%.2f,", data.floatValue);
            break;
        case TEXT:
            printf("%s,", data.textValue);
            break;
        case NONE:
            printf("#,");
            break;
        default:
            break;
        }
    } else {
        printf("#,");
    }

}
