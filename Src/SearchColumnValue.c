#include <stdlib.h>
#include <string.h>
#include "Database.h"
#include "DatabaseAPI.h"

#define OR_MAX 20
#define ROW_MAX 20
#define MAX 20  //归并后AND数组的最大数目
#define OPER_AMOUNT 8

#define LENGTH 25

int getColumnAmount(Table *table);
static int handleAnd(Table *table, Condition *condition, int *andRowNumber, int andRowAmount);
static int handleOr(Table *table, Condition *condition, int *orRowNumber);
static int selectMatchedRow(ColumnValue **columnsValue, COLUMN_TYPE columnType, int *rowNumber, int rowAmount,
                            Condition *condition);
static int match(ColumnValue *columnValue, COLUMN_TYPE columnType, Condition *condition);
static int eqOperation(Data data, Value *value);
static int neOperation(Data data, Value *value);
static int gtOperation(Data data, Value *value);
static int ltOperation(Data data, Value *value);
static int getOperation(Data data, Value *value);
static int letOperation(Data data, Value *value);
static int betweenOperation(Data data, Value *value);
static int likeOperation(Data data, Value *value);
static int matchRegex(const char *string, const char *pattern);
static int mergeAndOr(int (*andRowNumber)[ROW_MAX], int *andRowAmount, int andCount, int (*orRowNumber)[ROW_MAX],
                      int *orRowAmount, int orCount, int *resultRowNumber);
static int insertToResult(int *rowNumber, int rowAmount, int *result, int resultAmount);
static int inResult(int number, int *result, int resultAmount);

static int (*operations[8])(Data , Value *) = {
    eqOperation, neOperation, gtOperation, ltOperation,
    getOperation, letOperation, betweenOperation, likeOperation
};

int getResultColumnValue(Table *table, Column **selectedColumn, int selColumnAmount,
                   ColumnValue **resultColumnValue, Condition *condition, int isInner)
{
    int andRowNumber[MAX][ROW_MAX];
    int andRowAmount[MAX];
    int orRowNumber[MAX][ROW_MAX];
    int orRowAmount[MAX];
    int columnAmount = getColumnAmount(table);
    int i, j;
    int index = 0;
    if (condition == NULL)
    {
        for (i = 0; i < selColumnAmount; i++)
            resultColumnValue[i] = selectedColumn[i]->columnValueHead;
        if (resultColumnValue[0] == NULL)
            i = 0;
        else
            for (i = 1; ; i++)
            {
                for (j = 0; j < selColumnAmount; j++)
                {
                    index = i * selColumnAmount + j;
                    resultColumnValue[index] = resultColumnValue[index-selColumnAmount]->next;
                }
                if (resultColumnValue[index]->next == NULL)
                    break;
            }
        return i;
    }
    for (i = 0; i < MAX; i++)
    {
        andRowAmount[i] = columnAmount;
        for (j = 0; j < columnAmount; j++)
            andRowNumber[i][j] = j;
    }


    Condition *conditionTra = condition;
    Condition *prior = condition;

    int andCount = 0;
    int orCount = 0;
    while (conditionTra != NULL)
    {
        while (conditionTra->logic == AND)
        {
            andRowAmount[andCount] = handleAnd(table, conditionTra, andRowNumber[andCount],
                                                   andRowAmount[andCount]);
            if (andRowAmount[andCount] == -1)
                return -1;
            prior = conditionTra;
            conditionTra = conditionTra->next;
        }
        if (prior->logic == AND)
        {
            andRowAmount[andCount] = handleAnd(table, conditionTra, andRowNumber[andCount],
                                                   andRowAmount[andCount]);
            if (andRowAmount[andCount] == -1)
                return -1;
            andCount++;
        }
        else
        {
            orRowAmount[orCount] = handleOr(table, conditionTra, orRowNumber[orCount]);
            if (orRowAmount[orCount] == -1)
                return -1;
            orCount++;
        }
        prior = conditionTra;
        conditionTra = conditionTra->next;
    }

    int resultRowNumber[ROW_MAX];
    int resultRowAmount;
    resultRowAmount = mergeAndOr(andRowNumber, andRowAmount, andCount, orRowNumber, orRowAmount,
                                 orCount, resultRowNumber);

    ColumnValue *columnValueTra;
    ColumnValue *columnsValueTra[selColumnAmount];
    int curColumnValuePos = 0;
    for (i = 0; i < selColumnAmount; i++)
        columnsValueTra[i] = selectedColumn[i]->columnValueHead;

    if (isInner)
    {
        if (resultRowAmount == 1)
        {
            columnValueTra = (*selectedColumn)->columnValueHead;
            for (i = 0; i < resultRowNumber[0]; i++)
                columnValueTra = columnValueTra->next;
            *resultColumnValue = columnValueTra;
        }
    } else {
        for (i = 0; i < resultRowAmount; i++)
        {
            while (curColumnValuePos < resultRowNumber[i])
            {
                for (j = 0; j < selColumnAmount; j++)
                    columnsValueTra[j] = columnsValueTra[j]->next;
                curColumnValuePos++;
            }
            for (j = 0; j < selColumnAmount; j++)
                resultColumnValue[i*selColumnAmount+j] = columnsValueTra[j];
        }
    }

//    COLUMN_TYPE columnType[selColumnAmount];
//    for (i = 0; i < selColumnAmount; i++)
//        columnType[i] = selectedColumn[i]->columnType;
//    for (i = 0; i < resultRowAmount; i++)
//        for (j = 0; j < selColumnAmount; j++)
//            outputValue(resultColumnValue[i*selColumnAmount+j], columnType[j]);
    return resultRowAmount;
}
static int handleAnd(Table *table, Condition *condition, int *andRowNumber, int andRowAmount)
{
    Column *column = searchColumn(table, condition->columnName, NULL);
    if (column == NULL)
        return -1;

    int i;
    ColumnValue *columnsValue[ROW_MAX];
    ColumnValue *columnValueTra = column->columnValueHead;
    int columnsValuePos = 0;

    for (i = 0; i < andRowAmount; i++)
    {
        while (columnsValuePos++ < andRowNumber[i])
            columnValueTra = columnValueTra->next;
        columnsValue[i] = columnValueTra;
    }

    int newAndRowAmount = selectMatchedRow(columnsValue, column->columnType, andRowNumber, andRowAmount, condition);
    return newAndRowAmount;
}
int getColumnAmount(Table *table)
{
    if (table == NULL)
        return 0;
    Column *columnTra = table->columnHead;
    int i;
    for (i = 0; columnTra != NULL; i++)
        columnTra = columnTra->next;
    return i;
}
static int handleOr(Table *table, Condition *condition, int *orRowNumber)
{
    Column *column = searchColumn(table, condition->columnName, NULL);
    if (column == NULL)
        return -1;

    int count = 0;
    ColumnValue *columnValueTra = column->columnValueHead;
    int isMatch;
    int i = 0;
    while (columnValueTra != NULL)
    {
        isMatch = match(columnValueTra, column->columnType, condition);
        if (isMatch == -1)
            return -1;
        if (isMatch)
            orRowNumber[count++] = i;
        columnValueTra = columnValueTra->next;
        i++;
    }
    return count;
}
static int selectMatchedRow(ColumnValue **columnsValue, COLUMN_TYPE columnType, int *rowNumber, int rowAmount,
                            Condition *condition)
{
    int i;
    int count = 0;
    int isMatch = 0;
    for (i = 0; i < rowAmount; i++)
    {
        isMatch = match(columnsValue[i], columnType, condition);
        if (isMatch == -1)
            return -1;
        if (isMatch)
            rowNumber[count++] = rowNumber[i];
    }
    return count;
}
static int match(ColumnValue *columnValue, COLUMN_TYPE columnType, Condition *condition)
{
    if (condition->value.columnType != columnType)
        return -1;
    int operNumber = condition->operator;
    Value value[2] = {condition->value, condition->value2};
    if (columnValue->hasData == 0)
        return 0;
    int result = operations[operNumber](columnValue->data, value);
    return result;
}
static int eqOperation(Data data, Value *value)
{
    int result = 0;
    switch (value[0].columnType)
    {
    case INT:
        result = (data.intValue == value[0].columnValue.intValue);
        break;
    case FLOAT:
        result = (data.floatValue == value[0].columnValue.floatValue);
        break;
    case TEXT:
        result = !strcmp(data.textValue, value[0].columnValue.textValue);
        break;
    default:
        break;
    }
    return result;
}
static int neOperation(Data data, Value *value)
{
    int result = 0;
    switch (value[0].columnType)
    {
    case INT:
        result = (data.intValue != value[0].columnValue.intValue);
        break;
    case FLOAT:
        result = (data.floatValue != value[0].columnValue.floatValue);
        break;
    case TEXT:
        result = strcmp(data.textValue, value[0].columnValue.textValue);
        break;
    default:
        break;
    }
    return result;
}
static int gtOperation(Data data, Value *value)
{
    int result = 0;
    switch (value[0].columnType)
    {
    case INT:
        result = (data.intValue > value[0].columnValue.intValue);
        break;
    case FLOAT:
        result = (data.floatValue > value[0].columnValue.floatValue);
        break;
    case TEXT:
        result = (strcmp(data.textValue, value[0].columnValue.textValue) > 0);
        break;
    default:
        break;
    }
    return result;
}
static int ltOperation(Data data, Value *value)
{
    int result = 0;
    switch (value[0].columnType)
    {
    case INT:
        result = (data.intValue < value[0].columnValue.intValue);
        break;
    case FLOAT:
        result = (data.floatValue < value[0].columnValue.floatValue);
        break;
    case TEXT:
        result = (strcmp(data.textValue, value[0].columnValue.textValue) < 0);
        break;
    default:
        break;
    }
    return result;
}
static int getOperation(Data data, Value *value)
{
    int result = 0;
    switch (value[0].columnType)
    {
    case INT:
        result = (data.intValue >= value[0].columnValue.intValue);
        break;
    case FLOAT:
        result = (data.floatValue >= value[0].columnValue.floatValue);
        break;
    case TEXT:
        result = (strcmp(data.textValue, value[0].columnValue.textValue) >= 0);
        break;
    default:
        break;
    }
    return result;
}
static int letOperation(Data data, Value *value)
{
    int result = 0;
    switch (value[0].columnType)
    {
    case INT:
        result = (data.intValue <= value[0].columnValue.intValue);
        break;
    case FLOAT:
        result = (data.floatValue <= value[0].columnValue.floatValue);
        break;
    case TEXT:
        result = (strcmp(data.textValue, value[0].columnValue.textValue) <= 0);
        break;
    default:
        break;
    }
    return result;
}
static int betweenOperation(Data data, Value *value)
{
    int result = 0;
    switch (value[0].columnType)
    {
    case INT:
        result = (data.intValue > value[0].columnValue.intValue &&
                  data.intValue < value[1].columnValue.intValue);
        break;
    case FLOAT:
        result = (data.floatValue > value[0].columnValue.floatValue &&
                  data.floatValue < value[1].columnValue.floatValue);
        break;
    default:
        break;
    }
    return result;
}
static int likeOperation(Data data, Value *value)
{
    int result = 0;
    char *regexString;
    switch (value[0].columnType)
    {
    case INT:
    case FLOAT:
        break;
    case TEXT:
        regexString = value[0].columnValue.textValue;
        result = matchRegex(data.textValue, regexString);
        break;
    default:
        break;
    }
    return result;
}
static int matchRegex(const char *string, const char *pattern)
{
    if (*pattern == 0)
    {
        if (*string == 0)
            return 1;
        return 0;
    }

    if (*string == 0)
    {
        if (!strcmp(pattern, "*"))
            return 1;
        return 0;
    }
    if (*pattern != '*' && *pattern != '?')
    {
        if (*string != *pattern)
            return 0;
        else
            return matchRegex(string+1, pattern+1);
    }

    if (*pattern == '?')
        return matchRegex(string+1, pattern+1);

    return matchRegex(string+1, pattern) || matchRegex(string, pattern+1);
}
static int mergeAndOr(int (*andRowNumber)[ROW_MAX], int *andRowAmount, int andCount, int (*orRowNumber)[ROW_MAX],
                      int *orRowAmount, int orCount, int *resultRowNumber)
{
    int resultAmount = 0;
    int i;

    for (i = 0; i < andCount; i++)
        resultAmount = insertToResult(andRowNumber[i], andRowAmount[i], resultRowNumber, resultAmount);
    for (i = 0; i < orCount; i++)
        resultAmount = insertToResult(orRowNumber[i], orRowAmount[i], resultRowNumber, resultAmount);
    return resultAmount;
}
static int insertToResult(int *rowNumber, int rowAmount, int *result, int resultAmount)
{
    int i;
    for (i = 0; i < rowAmount; i++)
        if (!inResult(rowNumber[i], result, resultAmount))
            result[resultAmount++] = rowNumber[i];

    return resultAmount;
}
static int inResult(int number, int *result, int resultAmount)
{
    int i;
    for (i = 0; i < resultAmount; i++)
        if (result[i] == number)
            return 1;
    return 0;
}
