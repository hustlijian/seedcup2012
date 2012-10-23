#include <stdlib.h>
#include <string.h>
#include "Database.h"
#include "DatabaseAPI.h"

static int getColumnsValue(Table *table, char **columnsName, ColumnValue **columnValue,
                    COLUMN_TYPE *columnType, int size);
static int updateData(ColumnValue **columnsValue, COLUMN_TYPE *columnType, Value *newValues,
              int size);
static int isSameValue(ColumnValue *columnValue, Value oldValue);
static void assignData(ColumnValue *columnValue, Value newValue);
static int hasNoneType(Column **allColumn, int size);
static void initCurrent(Column **allColumn, ColumnValue **current, int size);
static void deleteFirstNode(Column **allColumn, int size);
static void deleteNode(ColumnValue **current, ColumnValue **prior, int size);
static void moveAllColumnPointer(ColumnValue **current, ColumnValue **prior, int size);
static int getInsertedColumnPos(Table *table, int *position, char **columnsName, int size);
static int insertColumnsValue(Column **columns, int columnSize, int *insertedColumnPos,
                              Value *values, int valueSize);
static int sameType(Column **columns, int *insertedColumnPos, Value *values, int size);
static int searchPos(int *insertedColumnPos, int key, int size);

#define SIZE 20

extern Database *head;
extern Database *currentDatabase;

int select(SelectBody *selectBody, int *rowAmount)
{
    return 0;
}
int update(UpdateBody *updateBody)
{
    Table *table = searchTable(updateBody->tableName);
    Column *updatedColumn = searchColumn(table, updateBody->updatedColumn,
                                  NULL);
    if (updatedColumn == NULL)
        return -1;

    int size = updateBody->columnAmount;
    ColumnValue *columnsValue[size];
    COLUMN_TYPE columnType[size];
    int hasBadColumnName = 0;
    hasBadColumnName = getColumnsValue(table, updateBody->columnsName,
                                       columnsValue, columnType, size);
    if (hasBadColumnName == 1)
        return -1;

    ColumnValue *columnValueTra = updatedColumn->columnValueHead;
    int findColumnValue;
    int result = 0;
    int i;

    if (updatedColumn->columnType != updateBody->oldValue.columnType) //类型不匹配
        return -1;
    while (columnValueTra != NULL && result != -1)
    {
        findColumnValue = isSameValue(columnValueTra, updateBody->oldValue);
        if (findColumnValue)
            result = updateData(columnsValue, columnType, updateBody->newValues,
                                size);

         for (i = 0; i < size; i++)
            columnsValue[i] = columnsValue[i]->next;
        columnValueTra = columnValueTra->next;
    }
    return result;
}
int delete(char *tableName, char *columnName, Value value)
{
    Table *table = searchTable(tableName);
    Column *column = searchColumn(table, columnName, NULL);
    if (column == NULL)
        return -1;
    if (column->columnType != value.columnType) //类型不匹配
        return -1;

    Column *allColumn[SIZE];
    int size;
    size = getAllColumn(table, allColumn, SIZE);
    if (hasNoneType(allColumn, size))
        return -1;

    ColumnValue *columnValueTra = column->columnValueHead;
    ColumnValue *current[SIZE];
    ColumnValue *prior[SIZE];
    int atFirstNode = 1;

    initCurrent(allColumn, current, size);

    while (columnValueTra != NULL)
    {
        if (isSameValue(columnValueTra, value))
        {
            if (atFirstNode)
                deleteFirstNode(allColumn, size);
            else
                deleteNode(current, prior, size);
        }
        else
            atFirstNode = 0;
        moveAllColumnPointer(current, prior, size);
        columnValueTra = columnValueTra->next;
    }
    return 0;
}
int insert(char *tableName, char **columnsName, Value *values, int amount)
{
    Table *table = searchTable(tableName);
    if (table == NULL)
        return -1;

    Column *allColumn[SIZE];
    int size;
    size = getAllColumn(table, allColumn, SIZE);
    if (amount > size)
        return -1;


    int insertedColumnPos[amount];
    int hasBadColumnName = 0;

    if (columnsName != NULL)
        hasBadColumnName = getInsertedColumnPos(table, insertedColumnPos, columnsName,
                                                amount);
    if (hasBadColumnName)
        return -1;

    int result;
    result = insertColumnsValue(allColumn, size, insertedColumnPos, values, amount);
    return result;
}
static int getColumnsValue(Table *table, char **columnsName, ColumnValue **columnValue,
                    COLUMN_TYPE *columnType, int size)
{
    if (table == NULL)
        return 1;

    Column *columnHead = table->columnHead;
    Column *columnTra;
    int i;

    for (i = 0; i < size; i++)
    {
        columnTra = columnHead;
        while (columnTra != NULL)
        {
            if (!strcasecmp(columnsName[i], columnTra->columnName))
            {
                columnValue[i] = columnTra->columnValueHead;
                columnType[i] = columnTra->columnType;
                break;
            }
            columnTra = columnTra->next;
        }
        if (columnTra == NULL)
            return 1;
    }
    return 0;
}
static int isSameValue(ColumnValue *columnValue, Value oldValue)
{
    int result = 0;
    switch (oldValue.columnType)
    {
    case INT:
        if (columnValue->data.intValue == oldValue.columnValue.intValue)
            result = 1;
        break;
    case FLOAT:
        if (columnValue->data.floatValue == oldValue.columnValue.floatValue)
            result = 1;
        break;
    case TEXT:
        if (!strcmp(columnValue->data.textValue, oldValue.columnValue.textValue))
            result = 1;
        break;
    default:
        break;
    }
    return result;
}
static int updateData(ColumnValue **columnsValue, COLUMN_TYPE *columnType, Value *newValues,
              int size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        if (newValues[i].columnType != EMPTY
            && columnType[i] != newValues[i].columnType) //如果类型不匹配
            return -1;
        assignData(columnsValue[i], newValues[i]);
    }
    return 0;
}
static void assignData(ColumnValue *columnValue, Value newValue)
{
    switch (newValue.columnType)
    {
    case INT:
        columnValue->data.intValue = newValue.columnValue.intValue;
        columnValue->hasData = 1;
        break;
    case FLOAT:
        columnValue->data.floatValue = newValue.columnValue.floatValue;
        columnValue->hasData = 1;
        break;
    case TEXT:
        if (columnValue->data.textValue == NULL)
            columnValue->data.textValue = calloc(LENGTH, sizeof(char));
        strncpy(columnValue->data.textValue, newValue.columnValue.textValue, LENGTH);
        columnValue->hasData = 1;
        break;
    default:
        columnValue->hasData = 0;
        break;
    }
}
int getAllColumn(Table *table, Column **allColumn, int maxSize)
{
    if (table == NULL)
        return 0;
    Column *columnTra = table->columnHead;
    int i;
    int count = 0;
    for (i = 0; i < maxSize && columnTra != NULL; i++)
    {
        allColumn[count] = columnTra;
        columnTra = columnTra->next;
        count++;
    }
    return count;
}
static int hasNoneType(Column **allColumn, int size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        if (allColumn[i]->columnType == NONE)
            return 1;
    }
    return 0;
}
static void initCurrent(Column **allColumn, ColumnValue **current, int size)
{
    int i;
    for (i = 0; i < size; i++)
            current[i] = allColumn[i]->columnValueHead;
}
static void deleteFirstNode(Column **allColumn, int size)
{
    int i;
    ColumnValue *column;
    for (i = 0; i < size; i++)
    {
        column = allColumn[i]->columnValueHead;
        allColumn[i]->columnValueHead = column->next;
    }

}
static void deleteNode(ColumnValue **current, ColumnValue **prior, int size)
{
    int i;
    for (i = 0; i < size; i++)
        prior[i]->next = current[i]->next;
}
static void moveAllColumnPointer(ColumnValue **current, ColumnValue **prior, int size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        prior[i] = current[i];
        current[i] = current[i]->next;
    }
}
static int getInsertedColumnPos(Table *table, int *position, char **columnsName, int size)
{
    if (table == NULL)
        return -1;
    Column *columnTra;
    int i;
    int count;
    for (i = 0; i < size ; i++)
    {
        columnTra = table->columnHead;
        count = 0;
        while (columnTra != NULL)
        {
            //printf("%s\t%s\n", columnTra->columnName, columnsName[i]);
            if (!strcasecmp(columnTra->columnName, columnsName[i]))
            {
                position[i] = count;
                break;
            }
            columnTra = columnTra->next;
            count++;
        }
        if (columnTra == NULL)
            return 1;
    }
    return 0;
}
static int insertColumnsValue(Column **columns, int columnSize, int *insertedColumnPos,
                              Value *values, int valueSize)
{
    int isSameType = sameType(columns, insertedColumnPos, values, valueSize);
    if (!isSameType)
        return -1;

    int i;
    int count = 0;
    ColumnValue *newColumnsValue[columnSize];
    ColumnValue *columnsValueTra[columnSize];;
    for (i = 0; i < columnSize; i++)
    {
        newColumnsValue[i] = calloc(1, sizeof(ColumnValue));
        newColumnsValue[i]->next = NULL;
        if (searchPos(insertedColumnPos, i, valueSize))
            assignData(newColumnsValue[i], values[count++]);
        columnsValueTra[i] = columns[i]->columnValueHead;;
    }

    if (columnsValueTra[0] == NULL)  //表内无数据
        for (i = 0; i < columnSize; i++)
            columns[i]->columnValueHead = newColumnsValue[i];
    else
    {
        while (columnsValueTra[0]->next != NULL)
            for (i = 0; i < columnSize; i++)
                columnsValueTra[i] = columnsValueTra[i]->next;
        for (i = 0; i < columnSize; i++)
            columnsValueTra[i]->next = newColumnsValue[i];
    }
    return 0;
}
static int sameType(Column **columns, int *insertedColumnPos, Value *values, int size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        if (values [i].columnType != EMPTY &&
            columns[insertedColumnPos[i]]->columnType != values[i].columnType)
            return 0;
    }
    return 1;
}
static int searchPos(int *insertedColumnPos, int key, int size)
{
    int i;
    for (i = 0; i < size; i++)
        if (key == insertedColumnPos[i])
            return 1;
    return 0;
}
