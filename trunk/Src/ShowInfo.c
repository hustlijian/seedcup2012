#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Database.h"

#define SIZE 20

int descOrderCompare(const void *elem1, const void *elem2);
int incrOrderCompare(const void *elem1, const void *elem2);
void sortStringArray(char **stringArray, int size,
                     SORT_ORDER sortOrder);
void outputStringArray(char **stringArray, int size);
void freeStringArray(char **stringArray, int size);
char *enumToString(COLUMN_TYPE columnType);

extern Database *head;
extern Database *currentDatabase;

void showDatabase(SORT_ORDER sortOrder)
{
    char *databasesName[SIZE];
    memset(databasesName, 0, SIZE*sizeof(char *));
    Database *databaseTra = head;
    int count = 0;

    if (databaseTra == NULL)
    {
        printf("$\n");   //NO DATA
        return ;
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
}
void showTable(char *databaseName, SORT_ORDER sortOrder)
{
     //databaseName为NULL时指定为当前数据库
    Database *database;

    if (databaseName == NULL)
        database = currentDatabase;
    else
        database = searchDatabase(databaseName, NULL);
    if (database == NULL)
    {
        printf("$\n");
        return ;
    }

    char *tablesName[SIZE];
    memset(tablesName, 0, SIZE*sizeof(char *));
    Table *tableTra = database->tableHead;
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
}
void showColumn(char *tableName, SORT_ORDER sortOrder)
{
    Table *table = searchTable(tableName);
    if (table == NULL)
    {
        printf("$\n");
        return ;
    }

    char *columnsName[SIZE];
    memset(columnsName, 0, SIZE*sizeof(char *));
    Column *columnTra = table->columnHead;
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
}
void showColumnType(char *tableName)
{
    Table *table = searchTable(tableName);
    if (table == NULL)
    {
        printf("$\n");
        return ;
    }
    Column *columnTra = table->columnHead;
    while (columnTra != NULL)
    {
        printf("%s, ", enumToString(columnTra->columnType));
        columnTra = columnTra->next;
    }
    putchar('\n');
}
void sortStringArray(char **stringArray, int size, SORT_ORDER sortOrder)
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
int descOrderCompare(const void *elem1, const void *elem2)
{
    return -strcmp(*(char **)elem1, *(char **)elem2);
}
int incrOrderCompare(const void *elem1, const void *elem2)
{
    return strcmp(*(char **)elem1, *(char **)elem2);
}
void outputStringArray(char **stringArray, int size)
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
void freeStringArray(char **stringArray, int size)
{
    int i;
    for (i = 0; i < size; i++)
        free(stringArray[i]);
}

char *enumToString(COLUMN_TYPE columnType)
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
