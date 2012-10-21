#include <stdlib.h>
#include <string.h>
#include "Database.h"

static void freeAllColumnValue(ColumnValue *columnValue, int isTextType);
static int canAlterColumnType(COLUMN_TYPE oldType, COLUMN_TYPE newType);

void alterToInt(Data *data, COLUMN_TYPE oldColumnType);
void alterToFloat(Data *data, COLUMN_TYPE oldColumnType);
void alterToText(Data *data, COLUMN_TYPE oldColumnType);
void alterToNone(Data *data, COLUMN_TYPE oldColumnType);
void alterColumnValue(ColumnValue *columnValue, void (*alterData)(Data *,
                            COLUMN_TYPE), COLUMN_TYPE oldColumnType);

void freeAllColumn(Column *column, int isFreeColumn);

int dropDatabase(char *databaseName);
int dropOneTable(char *databaseName, char *tableName);
Database *searchDatabase(char *databaseName);
void dropAllTable(Table *tableTraverse);
void freeTable(Table *table);


Database *head = NULL;
Database *currentDatabase = NULL;

int createDatabase(char *databaseName)
{
    Database *traverse = head;
    Database *newDatabase = (Database *)calloc(1, sizeof(Database));
    strncpy(newDatabase->databaseName, databaseName, LENGTH);
    newDatabase->next = NULL;
    newDatabase->tableHead = NULL;

    if (head == NULL)
        head = newDatabase;
    else
    {
        while (traverse->next != NULL)
        {
            if (!strcmp(databaseName, traverse->databaseName))
            {
                free(newDatabase);
                return -1;
            }
            traverse = traverse->next;
        }
        traverse->next = newDatabase;
    }
    return 0;
}
int createTable(char *tableName, char **columnsName,
                COLUMN_TYPE *columnType, int columnAmount)
{
    if (currentDatabase == NULL)
        return -1;

    Table *traverse = currentDatabase->tableHead;
    Table *newTable = (Table *)calloc(1, sizeof(Table));
    strncpy(newTable->tableName, tableName, LENGTH-1);
    newTable->next = NULL;
    if (traverse == NULL)
        currentDatabase->tableHead = newTable;
    else
    {
        while (traverse->next != NULL)
        {
            if (!strcmp(tableName, traverse->tableName))
            {
                free(newTable);
                return -1;
            }
            traverse = traverse->next;
        }
        traverse->next = newTable;
    }

    int i;
    Column *newColumn;
    Column *prior;
    for (i = 0; i < columnAmount; i++)
    {
        newColumn = (Column *)calloc(1, sizeof(Column));
        if (i == 0)
            newTable->columnHead = newColumn;
        else
            prior->next = newColumn;
        prior = newColumn;
    }
    return 0;
}
int addColumn(char *tableName, char *columnName,
              COLUMN_TYPE columnType)
{
    if (currentDatabase == NULL)
        return -1;

    Table *tableTraverse = searchTable(tableName);
    if (tableTraverse == NULL)   //�ҵ���β����δ�ҵ�
        return -1;

    Column *newColumn = (Column *)calloc(1, sizeof(Column));
    strncpy(newColumn->columnName, columnName, LENGTH-1);
    newColumn->columnType = columnType;
    newColumn->next = NULL;
    newColumn->columnValueHead = NULL;

    Column *columnTraverse = tableTraverse->columnHead;
    if (columnTraverse == NULL)
        tableTraverse->columnHead = newColumn;
    else
    {
        while (columnTraverse->next != NULL)
        {
            if (!strcmp(columnName, columnTraverse->columnName))
            {
                free(newColumn);
                return -1;
            }
            columnTraverse = columnTraverse->next;
        }
        columnTraverse->next = newColumn;
    }

    ColumnValue *columnValueTra = columnTraverse->columnValueHead;
    ColumnValue *newColumnValue;
    ColumnValue *prior;

    while (columnValueTra != NULL)
    {
        newColumnValue = (ColumnValue *)calloc(1, sizeof(ColumnValue));
        if (columnType == TEXT)
            newColumnValue->data.textValue = (char *)calloc(LENGTH, sizeof(char));

        if (newColumn->columnValueHead == NULL)
            newColumn->columnValueHead = newColumnValue;
        else
            prior->next = newColumnValue;
        prior = newColumnValue;
        columnValueTra = columnValueTra->next;
    }
    newColumnValue->next = NULL;

    return 0;
}

int rmColumn(char *tableName, char *columnName)
{
    if (currentDatabase == NULL)
        return -1;

    Column *prior;
    Table *table = searchTable(tableName);
    Column *column = searchColumn(table, columnName, &prior);
    //Column *column = searchColumn(tableName, columnName, &prior);
    if (column == NULL)
        return -1;

    if (prior == table->columnHead)
        table->columnHead = column->next;
    else
        prior->next = column->next;
    freeAllColumnValue(column->columnValueHead, column->columnType==TEXT);
    free(column);
    return 0;
}
int alterColumn(char *tableName, char *columnName,
                COLUMN_TYPE newColumnType)
{
    Table *table = searchTable(tableName);
    Column *column = searchColumn(table, columnName, NULL);
    if (column == NULL)
        return -1;

    COLUMN_TYPE oldColumnType = column->columnType;
    if (!canAlterColumnType(oldColumnType, newColumnType))
        return -1;

    //���ڴ˺�����Ҫ�ı䵱ǰ�������е����ݣ�����ͬ����������ת����ӵ�ж�Ӧ
    //�Ĳ�ͬ�Ĵ�������������ͨ��switch����alterData����ָ�������ȷ�ĸ�
    //ֵȻ���䴫�ݸ�alterColumnValueʹ��
    void (*alterData)(Data *, COLUMN_TYPE);
    switch (newColumnType)
    {
    case INT:
        alterData = alterToInt;
        break;
    case FLOAT:
        alterData = alterToFloat;
        break;
    case TEXT:
        alterData = alterToText;
        break;
    default:  //NONE
        alterData = alterToNone;
        break;
    }
    alterColumnValue(column->columnValueHead, alterData, oldColumnType);

    return 0;
}
int truncateTable(char *tableName)
{
    Table *table = searchTable(tableName);
    if (table == NULL)
        return -1;

    //NOTE: ����ķ���ʹ�õݹ�ʵ�֣��п��ܻᵼ�³�������Ч�ʵ���
    freeAllColumn(table->columnHead, 0);
    return 0;
}
int use(char *databaseName)
{
    Database *traverse = head;
    while (traverse != NULL)
    {
        if (!strcmp(traverse->databaseName, databaseName))
        {
            currentDatabase = traverse;
            return 0;
        }
        traverse = traverse->next;
    }
    return -1;
}
int drop(char *databaseName, char *tableName)
{
    //tableNameΪNULLʱɾ������database
    int result;
    if (tableName == NULL)
        result = dropDatabase(databaseName);
    else
        result = dropOneTable(databaseName, tableName); //TODO
    return result;
}
int renameDatabase(char *oldName, char *newName)
{
    Database *database = searchDatabase(oldName);
    if (database == NULL)
        return -1;
    strncpy(database->databaseName, newName, LENGTH-1);
    return 0;
}
int renameTable(char *oldName, char *newName)
{
    Table *table = searchTable(oldName);
    if (table == NULL)
        return -1;

    strncpy(table->tableName, newName, LENGTH-1);
    return 0;
}

Table *searchTable(char *tableName)
{
    //��Ҫȷ��currentDatabase!=NULLʱ���ܵ��ô˺���
    Table *traverse = currentDatabase->tableHead;
    while (traverse != NULL && strcmp(traverse->tableName,
                                      tableName))
    traverse = traverse->next;
    return traverse;
}
Column *searchColumn(Table *table, char *columnName, Column **prior)
{
    if (table == NULL)
        return NULL;

    Column *columnTraverse = table->columnHead;
    Column *priorTra;
    while (columnTraverse != NULL && strcmp(columnTraverse->columnName, columnName))
    {
        priorTra = columnTraverse;
        columnTraverse = columnTraverse->next;
    }
    if (prior != NULL)
        *prior = priorTra;
    return columnTraverse;
}
static void freeAllColumnValue(ColumnValue *columnValue, int isTextType)
{
    if (columnValue == NULL)
        return;
    while (columnValue->next != NULL)
        freeAllColumnValue(columnValue->next, isTextType);
    if (isTextType)
        free(columnValue->data.textValue);
    free(columnValue);
}

static int canAlterColumnType(COLUMN_TYPE oldType, COLUMN_TYPE newType)
{
    //��ǰ�Ƿ��ж���oldType��newType�Ƿ���ȣ�
    if (oldType == newType || oldType == TEXT)   //TEXT���Ͳ���ת������������
        return 0;

    //INT��FLOAT���Ի���ת��������,NONE���Ը��κ������໥ת������TEXTת��NONEʱ
    //�ᶪʧTEXT��Ϣ
    return 1;
}
void alterToInt(Data *data, COLUMN_TYPE oldColumnType)
{
    data->intValue = (int)data->floatValue;
}
void alterToFloat(Data *data, COLUMN_TYPE oldColumnType)
{
    data->floatValue = (float)data->intValue;
}
void alterToText(Data *data, COLUMN_TYPE oldColumnType)
{
    data->textValue = (char *)calloc(LENGTH, sizeof(char));
    if (oldColumnType == INT)
        itoa(data->intValue, data->textValue, 10);
    else if (oldColumnType == FLOAT)
        gcvt(data->floatValue, LENGTH-1, data->textValue);
}
void alterToNone(Data *data, COLUMN_TYPE oldColumnType)
{
    if (oldColumnType == TEXT)
        free(data->textValue);
}
void alterColumnValue(ColumnValue *columnValue, void (*alterData)(Data *,
                            COLUMN_TYPE), COLUMN_TYPE oldColumnType)
{
    if (columnValue == NULL)
        return ;
    ColumnValue *columnValueTraverse = columnValue;
    Data *dataTraverse;

    while (columnValueTraverse != NULL)
    {
        dataTraverse = &columnValueTraverse->data;
        if (dataTraverse != NULL && alterData != NULL)
            alterData(dataTraverse, oldColumnType);
        columnValueTraverse = columnValueTraverse->next;
    }
}


void freeAllColumn(Column *column, int isFreeColumn)
{
    //isFreeColumn�����Ƿ��ͷ�Column����
    if (column == NULL)
        return ;
    if (column->next != NULL)
        freeAllColumn(column->next, isFreeColumn);

    freeAllColumnValue(column->columnValueHead, column->columnType==TEXT);
    if (isFreeColumn)
        free(column);
}

 int dropDatabase(char *databaseName)
 {
    Database *database = searchDatabase(databaseName);
    if (database == NULL)
        return -1;

    Table *table = database->tableHead;
    dropAllTable(table);
    return 0;
 }
int dropOneTable(char *databaseName, char *tableName)
{
    Database *database = searchDatabase(databaseName);
    if (database == NULL)
        return -1;

    Table *tableTraverse = database->tableHead;
    Table *prior =tableTraverse;

    while (tableTraverse != NULL)
    {
        if (!strcmp(tableName, tableTraverse->tableName))
            break;
        prior = tableTraverse;
        tableTraverse = tableTraverse->next;
    }
    if (tableTraverse == NULL)
        return -1;

    if (prior == database->tableHead)
        database->tableHead = tableTraverse->next;
    else
        prior->next = tableTraverse->next;
    freeTable(tableTraverse);
    return 0;
}
Database *searchDatabase(char *databaseName)
{
    Database *databaseTra = head;
    while (databaseTra != NULL)
    {
        if (!strcmp(databaseTra->databaseName, databaseName))
        {
            currentDatabase = databaseTra;
            break;
        }
        databaseTra = databaseTra->next;
    }
    return databaseTra;
}
//NOTE:�������кܶ��εĵݹ麯�����������Ч�ʵ��£���Щ����
//�͵ÿ�����д
void dropAllTable(Table *tableTraverse)
{
    if (tableTraverse == NULL)
        return ;
    while (tableTraverse != NULL)
        dropAllTable(tableTraverse->next);
    freeTable(tableTraverse);
}
void freeTable(Table *table)
{
    if (table == NULL)
        return ;

    Column *columnTra = table->columnHead;
    freeAllColumn(columnTra, 1);
}
