#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Database.h"
#define OUTPUT_MAX 20

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

int showDatabase(SORT_ORDER sortOrder)
{
    char *databasesName[OUTPUT_MAX];
    memset(databasesName, 0, OUTPUT_MAX*sizeof(char *));
    Database *databaseTra = head;
    int count = 0;

    if (databaseTra == NULL)
    {
        printf("$\n");   //NO DATA
        return 0;
    }

    while (databaseTra != NULL)
    {
        databasesName[count] = calloc(LENGTH, sizeof(char));
        strncpy(databasesName[count++], databaseTra->databaseName, LENGTH-1);
        databaseTra = databaseTra->next;
    }
    sortStringArray(databasesName, count, sortOrder);
    outputStringArray(databasesName, count);
    freeStringArray(databasesName, count);
    return 0;
}
int showTable(char *databaseName, SORT_ORDER sortOrder)
{
     //databaseName为NULL时指定为当前数据库
    Database *database;

    if (databaseName == NULL)
        database = currentDatabase;
    else
        database = searchDatabase(databaseName, NULL);
    if (database == NULL)
        return -1;

    char *tablesName[OUTPUT_MAX];
    memset(tablesName, 0, OUTPUT_MAX*sizeof(char *));
    Table *tableTra = database->tableHead;
    if (tableTra == NULL)
    {
        printf("$\n");
        return 0;
    }
    int count = 0;

    while (tableTra != NULL)
    {
        tablesName[count] = calloc(LENGTH, sizeof(char));
        strncpy(tablesName[count++], tableTra->tableName, LENGTH-1);
        tableTra = tableTra->next;
    }
    sortStringArray(tablesName, count, sortOrder);
    outputStringArray(tablesName, count);
    freeStringArray(tablesName, count);
    return 0;
}
int showColumn(char *tableName, SORT_ORDER sortOrder)
{
    Table *table = searchTable(tableName);
    if (table == NULL)
        return -1;

    char *columnsName[OUTPUT_MAX];
    memset(columnsName, 0, OUTPUT_MAX*sizeof(char *));
    Column *columnTra = table->columnHead;
    if (columnTra == NULL)
    {
        printf("$\n");
        return 0;
    }
    int count = 0;

    while (columnTra != NULL)
    {
        columnsName[count] = calloc(LENGTH, sizeof(char));
        strncpy(columnsName[count++], columnTra->columnName, LENGTH-1);

        columnTra = columnTra->next;
    }
    sortStringArray(columnsName, count, sortOrder);
    outputStringArray(columnsName, count);
    freeStringArray(columnsName, count);
    return 0;
}
int showColumnType(char *tableName)
{
    Table *table = searchTable(tableName);
    if (table == NULL)
        return -1;

    Column *columnTra = table->columnHead;
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
int showAllColumnValue(char *tableName)
{
    Table *table = searchTable(tableName);
    if (table == NULL)
        return -1;

    Column *allColumn[OUTPUT_MAX];
    int length = getAllColumn(table, allColumn, OUTPUT_MAX);
    if (length <= 0)
        return -1;
    ColumnValue *columnValueTra[length];
    int i;

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
    if (sortOrder == NOTSORT)
        return ;
    int (*compare)(const void *, const void *);
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
