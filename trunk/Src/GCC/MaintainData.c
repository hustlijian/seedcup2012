 /**
 * @file    MaintainData.c
 * @author  hzhigeng <hzhigeng@gmail.com>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * 数据操作部分，用于完成SELECT、UPDATE、DELETE、INSERT INTO等相关功能。
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Database.h"
#include "DatabaseAPI.h"

#define ROW_MAX 20

extern int getResultColumnValue(Table *table, Column **selectedColumn, int selColumnAmount,
                   ColumnValue **resultColumnValue, Condition *condition, int isInner);

static int handleInnerSelect(SelectBody *selectBody);
static int getColumnAmount(Table *table);
static int handleOuterSelect(SelectBody *selectBody);
static void assignValue(Value *value, Data data, COLUMN_TYPE columnType);
static void outputResult(ColumnValue **columnsValue, COLUMN_TYPE *columnType, int columnAmount,
                        int rowAmount, int sortByWhich, SORT_ORDER sortOrder);
static void assignString(char *string, ColumnValue *columnValue, COLUMN_TYPE columnType);
static void sortStringArray(char **stringArray, int rowAmount, int columnAmount, int sortByWhich,
                       SORT_ORDER sortOrder);
static void getFinalOrder(char **string, int rowAmount, int columnAmount, SORT_ORDER sortOrder,
                          int *finalOrder);
static int descOrderCompare(const char *elem1, const char *elem2);
static int incrOrderCompare(const char *elem1, const char *elem2);
static int containsBlank(char *string);
static int getColumnsValue(Table *table, char **columnsName, ColumnValue **columnValue,
                    COLUMN_TYPE *columnType, int size);
static int updateData(ColumnValue **columnsValue, COLUMN_TYPE *columnType, Value *newValues,
              int size);
static int isSameValue(ColumnValue *columnValue, Value oldValue);
static void assignData(ColumnValue *columnValue, Value newValue);
static int getColumnNumber(Table *table, Column *column);
static int hasNoneType(Column **allColumn, int size);
static void initCurrent(Column **allColumn, ColumnValue **current, int size);
static void deleteFirstNode(Column **allColumn, int size);
static void freeColumnValue(ColumnValue *columnValue, int isText);
static void deleteNode(ColumnValue **current, ColumnValue **prior, int size, int isText);
static void moveAllColumnPointer(ColumnValue **current, ColumnValue **prior, int size);
static int getInsertedColumnPos(Table *table, int *position, char **columnsName, int size);
static int insertColumnsValue(Column **columns, int columnSize, int *insertedColumnPos,
                              Value *values, int valueSize);
static int sameType(Column **columns, int *insertedColumnPos, Value *values, int size);
static int searchPos(int *insertedColumnPos, int key, int size);

#define SIZE 20

extern Database *head;
extern Database *currentDatabase;

/**
 * <select功能>
 *
 * @param   selectBody    select操作所需的参数
 * @return  0代表操作成功，-1代表操作失败
 */
int select(SelectBody *selectBody)
{
    if (selectBody == NULL)
        return -1;
    int result;
    if (selectBody->isInner)
        result = handleInnerSelect(selectBody);
    else
        result = handleOuterSelect(selectBody);
    return result;
}
/**
 * <update功能>
 *
 * @param   updateBody    update操作所需的参数
 * @return  0代表操作成功，-1代表操作失败
 */
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
    int haveFoundOne = 0;

    if (updatedColumn->columnType != updateBody->oldValue.columnType) //类型不匹配
        return -1;
    while (columnValueTra != NULL && result != -1)
    {
        findColumnValue = isSameValue(columnValueTra, updateBody->oldValue);
        if (findColumnValue)
        {
            result = updateData(columnsValue, columnType, updateBody->newValues,
                                size);
            haveFoundOne = 1;
        }

         for (i = 0; i < size; i++)
            columnsValue[i] = columnsValue[i]->next;
        columnValueTra = columnValueTra->next;
    }
    if (!haveFoundOne)
        return -1;
    return result;
}
/**
 * <delete功能>
 *
 * @param   tableName    操作的表的名字
 * @param   columnName   where条件的列名
 * @param   value        where条件的数据
 * @return  0代表操作成功，-1代表操作失败
 */
int delete(char *tableName, char *columnName, Value value)
{
    Table *table = searchTable(tableName);
    Column *column = searchColumn(table, columnName, NULL);
    if (column == NULL)
        return -1;
    if (value.columnType == EMPTY || column->columnType != value.columnType) //类型不匹配
        return -1;

    Column *allColumn[SIZE];
    int size;
    size = getAllColumn(table, allColumn, SIZE);
    if (hasNoneType(allColumn, size))
        return -1;

    int columnNmber = getColumnNumber(table, column);
    ColumnValue *current[SIZE];
    ColumnValue *prior[SIZE];
    int atFirstNode = 1;
    int haveFoundOne = 0;
    int result = 0;

    initCurrent(allColumn, current, size);

    while (current[columnNmber] != NULL)
    {
        result = isSameValue(current[columnNmber], value);
        if (result)
        {
            haveFoundOne = 1;
            if (atFirstNode)
                deleteFirstNode(allColumn, size);
            else
                deleteNode(current, prior, size, value.columnType==TEXT);
        }
        else
        {
            atFirstNode = 0;
            moveAllColumnPointer(current, prior, size);
        }
    }
    if (!haveFoundOne)
        return -1;
    return 0;
}
/**
 * <insert功能>
 *
 * @param   tableName     操作的表的名字
 * @param   columnsName   插入列的名字
 * @param   values        插入的新数据
 * @param   amount        插入列的数目
 * @return  0代表操作成功，-1代表操作失败
 */
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
    int i;
    for (i = 0; i < amount; i++)
        insertedColumnPos[i] = i;

    if (columnsName != NULL)
        hasBadColumnName = getInsertedColumnPos(table, insertedColumnPos, columnsName,
                                                amount);
    if (hasBadColumnName)
        return -1;

    int result;
    result = insertColumnsValue(allColumn, size, insertedColumnPos, values, amount);
    return result;
}
static int handleInnerSelect(SelectBody *selectBody)
{
    Table *table = searchTable(selectBody->tableName);
    int columnAmount;
    char *columnName;

    if (selectBody->columnsName == NULL)
    {
        columnAmount = getColumnAmount(table);
        if (columnAmount != 1)
            return -1;
        columnName = table->columnHead->columnName;
    } else {
        columnName = selectBody->columnsName[0];
    }
    Column *selectedColumn = searchColumn(table, columnName, NULL);
    if (selectedColumn == NULL)
        return -1;

    ColumnValue *resultColumnsValue;
    int rowAmount;
    rowAmount = getResultColumnValue(table, &selectedColumn, 1, &resultColumnsValue,
                                     selectBody->condition, 1);
    if (rowAmount != 1)
        return -1;
    assignValue(selectBody->resultValue, resultColumnsValue->data,
                    selectedColumn->columnType);
    return 0;
}
static int getColumnAmount(Table *table)
{
    if (table == NULL)
        return 0;
    Column *columnTra = table->columnHead;
    int i;
    for (i = 0; columnTra != NULL; i++)
        columnTra = columnTra->next;
    return i;
}
static void assignValue(Value *value, Data data, COLUMN_TYPE columnType)
{
    value->columnType = columnType;
    switch (columnType)
    {
    case INT:
        value->columnValue.intValue = data.intValue;
        break;
    case FLOAT:
        value->columnValue.floatValue = data.floatValue;
        break;
    case TEXT:
        value->columnValue.textValue = calloc(NAME_MAX, sizeof(char));
        strncpy(value->columnValue.textValue, data.textValue, NAME_MAX-1);
        break;
    default:
        break;
    }
}
static int handleOuterSelect(SelectBody *selectBody)
{
    Table *table = searchTable(selectBody->tableName);
    if (table == NULL)
        return -1;
    int columnAmount;
    if (selectBody->columnsName == NULL)
        columnAmount = getColumnAmount(table);
    else
        columnAmount = selectBody->columnAmount;
    if (columnAmount == 0)
    {
        printf("$\n");
        return 0;
    }

    Column *selectedColumns[columnAmount];
    COLUMN_TYPE columnType[columnAmount];
    int i;
    int sortByWhich = -1;
    if (selectBody->sortColumnName == NULL)
    {
        sortByWhich = 0;
        selectBody->sortOrder = INCR;
    }


    if (selectBody->columnsName == NULL)
        getAllColumn(table, selectedColumns, columnAmount);
    else
        for (i = 0; i < columnAmount; i++)
        {
            selectedColumns[i] = searchColumn(table, selectBody->columnsName[i], NULL);
            if (selectedColumns[i] == NULL)
                return -1;
        }
    for (i = 0; i < columnAmount; i++)
    {
        columnType[i] = selectedColumns[i]->columnType;
        if (sortByWhich < 0 && !mystrcmp(selectedColumns[i]->columnName, selectBody->sortColumnName))
            sortByWhich = i;
    }
    if (sortByWhich < 0)
        return -1;

    ColumnValue *resultColumnsValue[columnAmount*ROW_MAX];
    int rowAmount;
    rowAmount = getResultColumnValue(table, selectedColumns, columnAmount, resultColumnsValue,
                                     selectBody->condition, 0);

    if (rowAmount == -1)
        return -1;
    if (rowAmount == 0)
    {
        printf("$\n");
        return 0;
    }
    outputResult(resultColumnsValue, columnType, columnAmount, rowAmount, sortByWhich,
                 selectBody->sortOrder);
    return 0;
}
static void outputResult(ColumnValue **columnsValue, COLUMN_TYPE *columnType, int columnAmount,
                        int rowAmount, int sortByWhich, SORT_ORDER sortOrder)
{
    char *stringArray[rowAmount*columnAmount];
    int i, j;


    for (i = 0; i < rowAmount; i++)
    {
        for (j = 0; j < columnAmount; j++)
        {
            stringArray[i*columnAmount+j] = calloc(NAME_MAX, sizeof(char));
            assignString(stringArray[i*columnAmount+j], columnsValue[i*columnAmount+j], columnType[j]);
        }
    }
    sortStringArray(stringArray, rowAmount, columnAmount, sortByWhich, sortOrder);

    char *stringTemp;
    for (i = 0; i < rowAmount; i++)
    {
        for (j = 0; j < columnAmount; j++)
        {
            stringTemp = stringArray[i*columnAmount+j];
            if (containsBlank(stringTemp))
                printf("\"%s\"", stringTemp);
            else
                printf("%s", stringTemp);
            if (j != columnAmount - 1)
                putchar(',');
        }
        putchar('\n');
    }
    for (i = 0; i < rowAmount*columnAmount; i++)
        free(stringArray[i]);
}
static void assignString(char *string, ColumnValue *columnValue, COLUMN_TYPE columnType)
{
    if (columnValue->hasData)
    {
        switch (columnType)
        {
        case INT:
            sprintf(string, "%d", columnValue->data.intValue);
            break;
        case FLOAT:
            sprintf(string, "%.2f", columnValue->data.floatValue);
            break;
        case TEXT:
            strncpy(string, columnValue->data.textValue, NAME_MAX-1);
            break;
        case NONE:
            strcpy(string, "#");
            break;
        default:
            break;
        }
    } else {
        strcpy(string, "#");
    }

}
static void sortStringArray(char **stringArray, int rowAmount, int columnAmount, int sortByWhich,
                       SORT_ORDER sortOrder)
{
    if (sortOrder == NOTSORT)
        return ;

    int i, j;
    int finalOrder[rowAmount];

    for (i = 0; i < rowAmount; i++)
        finalOrder[i] = i;

    getFinalOrder(stringArray+sortByWhich, rowAmount, columnAmount, sortOrder, finalOrder);


    char *stringTemp[rowAmount*columnAmount];
    int finalRow;
    for (i = 0; i < rowAmount*columnAmount; i++)
        stringTemp[i] = stringArray[i];
    for (i = 0; i < rowAmount; i++)
    {
        for (j = 0; j < columnAmount; j++)
        {
            finalRow = finalOrder[i];
            stringArray[i*columnAmount+j] = stringTemp[finalRow*columnAmount+j];
        }
    }
}
static void getFinalOrder(char **string, int rowAmount, int columnAmount, SORT_ORDER sortOrder,
                          int *finalOrder)
{
    int i, j;
    int (*compare)(const char *, const char *);
    int swapTemp;
    char *stringSwapTemp;
    char *sortedStrings[rowAmount];

    if (sortOrder == DESC)
        compare = descOrderCompare;
    else
        compare = incrOrderCompare;
    for (i = 0; i < rowAmount; i++)
        sortedStrings[i] = string[i*columnAmount];

    for (i = 0; i < rowAmount; i++)
    {
        for (j = 0; j < rowAmount - 1; j++)
        {
            if (compare(sortedStrings[j], sortedStrings[j+1]) > 0)
            {
                swapTemp = finalOrder[j];
                finalOrder[j] = finalOrder[j+1];
                finalOrder[j+1] = swapTemp;

                stringSwapTemp = sortedStrings[j];
                sortedStrings[j] = sortedStrings[j+1];
                sortedStrings[j+1] = stringSwapTemp;
            }
        }
    }
}
static int descOrderCompare(const char *elem1, const char *elem2)
{
    if (!strcmp(elem1, "#"))
        return 1;
    if (!strcmp(elem2, "#"))
        return -1;
    return -strcmp(elem1, elem2);
}
static int incrOrderCompare(const char *elem1, const char *elem2)
{
    if (!strcmp(elem1, "#"))
        return -1;
    if (!strcmp(elem2, "#"))
        return 1;
    return strcmp(elem1, elem2);
}
static int containsBlank(char *string)
{
    char c;
    int i = 0;
    while ((c = string[i++]) != '\0')
        if (c == ' ')
            return 1;
    return 0;
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
            if (!mystrcmp(columnsName[i], columnTra->columnName))
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
    if (columnValue->hasData == 0)
        return 0;
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
            columnValue->data.textValue = calloc(NAME_MAX, sizeof(char));
        strncpy(columnValue->data.textValue, newValue.columnValue.textValue, NAME_MAX);
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
static int getColumnNumber(Table *table, Column *column)
{
    Column *columnTra = table->columnHead;
    int count = 0;
    while (columnTra != NULL && columnTra != column)
    {
        count++;
        columnTra = columnTra->next;
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
    ColumnValue *columnValue;
    for (i = 0; i < size; i++)
    {
        columnValue = allColumn[i]->columnValueHead;
        allColumn[i]->columnValueHead = columnValue->next;
        freeColumnValue(columnValue, allColumn[i]->columnType==TEXT);
    }

}
static void freeColumnValue(ColumnValue *columnValue, int isText)
{
    if (isText)
        free(columnValue->data.textValue);
    free(columnValue);
}
static void deleteNode(ColumnValue **current, ColumnValue **prior, int size, int isText)
{
    int i;
    ColumnValue *temp;
    for (i = 0; i < size; i++)
    {
        temp = current[i];
        current[i] = current[i]->next;
        prior[i]->next = current[i];
        freeColumnValue(temp, isText);
    }

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
            if (!mystrcmp(columnTra->columnName, columnsName[i]))
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
