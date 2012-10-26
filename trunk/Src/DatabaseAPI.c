/*
 *用来测试API调用
*/
#include <stdio.h>
#include "DatabaseAPI.h"

/*在返回值为int的函数中，返回-1代表失败，返回0代表成功*/
int createDatabase(char *databaseName)
{
	printf("function:createDatabase\n");
	printf("databaseName: %s\n", databaseName);
	return 0;
}
int createTable(char *tableName, char **columnsName, COLUMN_TYPE *columnType, int columnAmount)
{
	int i = 0;
	printf("function:createTable\n");
	printf("tableName:%s\n", tableName);
	
	for (i=0; i<columnAmount; i++)
	{
		printf("columnsName:%s ", *(columnsName+i));
	
		switch (columnType[i] )
		{
		case INT:
			printf("columnType:int ");
			break;
		case FLOAT:
			printf("columnType:float ");
			break;
		case TEXT:
			printf("columnType:text ");
			break;
		case NONE:
			printf("columnType:none ");
			break;
		case EMPTY:
			printf("columnType:empty ");
			break;
		default:
			printf("columnType:unknown ");
			break;
		}
		printf("\n");
	}
	printf("columnAmount:%d ", columnAmount);
	return 0;
}
int addColumn(char *tableName, char *columnName, COLUMN_TYPE columnType)
{
	printf("function: addColumn\n ");
	printf("columnName:%s\n", columnName);
	switch (columnType)
	{
	case INT:
		printf("columnType:int\n");
		break;
	case FLOAT:
		printf("columnType:float\n");
		break;
	case TEXT:
		printf("columnType:text\n");
		break;
	case NONE:
		printf("columnType:none\n");
		break;
	case EMPTY:
		printf("columnType:empty\n");
		break;
	default:
		printf("columnType:unknown\n");
		break;
	}
	return 0;
}
int rmColumn(char *tableName, char *columnName)
{
	printf("function: rmColumn\n");
	printf("columnName:%s\n", columnName);
	return 0;
}
int alterColumn(char *tableName, char *columnName, COLUMN_TYPE newColumnType)
{
	printf("function:alterColumn\n");
	printf("columnName:%s\n", columnName);
	switch (newColumnType)
	{
	case INT:
		printf("newColumnType:int\n");
		break;
	case FLOAT:
		printf("newColumnType:float\n");
		break;
	case TEXT:
		printf("newColumnType:text\n");
		break;
	case NONE:
		printf("newColumnType:none\n");
		break;
	case EMPTY:
		printf("newColumnType:empty\n");
		break;
	default:
		printf("columnType:unknown\n");
		break;
	}
	return 0;
}
int truncateTable(char *tableName)
{
	printf("function: truncateTable\n");
	printf("tableName:%s\n", tableName);
	return 0;
}
int use(char *databaseName)
{
	printf("function: use\n");
	printf("databaseName:%s\n", databaseName);
	return 0;
}
int drop(char *databaseName, char *tableName) 	//tableName为NULL时删除整个database
{
	printf("function: drop\n");
	printf("databaseName:%s\n", databaseName);
	if (tableName)
		printf("tableName:%s\n", tableName);
	return 0;
}
int renameDatabase(char *oldName, char *newName)
{
	printf("function: renameDatabase\n");
	printf("oldName:%s\n", oldName);
	printf("newName:%s\n", newName);
	return 0;
}
int renameTable(char *oldName, char *newName)
{
	printf("function: renameTable\n");
	printf("oldName:%s\n", oldName);
	printf("newName:%s\n", newName);
	return 0;
}
void showValue(Value *value)
{
	printf("columnType = ");
	switch(value->columnType)
	{
	case  INT:
		printf("INT\n");
		printf("columnValue = %d\n", value->columnValue.intValue);
		break;
	case FLOAT:
		printf("FLOAT\n");
		printf("columnValue = %.2f\n", value->columnValue.floatValue);
		break;
	case TEXT:
		printf("TEXT\n");
		printf("columnValue = %s\n", value->columnValue.textValue);
		break;
	case NONE:
		printf("NONE\n");
		break;
	case EMPTY:
		printf("EMPTY\n");
		break;
	default:
		break;
	}	
}
void showCondition(Condition *condition)
{
	printf("columnName = %s\n", condition->columnName);
	printf("value:\n");
	showValue(&(condition->value));
	if (condition->operator == BETWEEN)
	{
		printf("value2:\n");
		showValue(&(condition->value2));
	}
	switch(condition->operator)
	{
	case EQ:
		printf("EQ\n");
		break;
	case NE:
		printf("NE\n");
		break;
	case GT:
		printf("GT\n");
		break;
	case LT:
		printf("LT\n");
		break;
	case GET:
		printf("GET\n");
		break;
	case LET:
		printf("LET\n");
		break;
	case BETWEEN:
		printf("BETWEEN\n");
		break;
	case LIKE:
		printf("LIKE\n");
		break;
	default:
		printf("default\n");
		break;
	}
	switch(condition->logic)
	{
	case OR:
		printf("OR\n");
		break;
	case AND:
		printf("AND\n");
		break;
	case NOLOGIC:
		printf("NOLOGIC\n");
		break;
	default:
		printf("default\n");
		break;
	}
}
//select函数接口可能需要重写，由于Select可能产生多行数据，每行数据即是一个Value*数组，
//所以我考虑返回Value*型的二维数组，其中rowAmount指针指向的值记录了行数
int select(SelectBody *selectBody)
{
	int i;
	Condition  *p;
	printf("function: select\n");
	printf("tableName:%s\n", selectBody->tableName);
	printf("columnAmount = %d\n", selectBody->columnAmount);
	for (i=0;i<selectBody->columnAmount;i++)
	{
		printf("columnsName[i] = %s\n", selectBody->columnsName[i]);
		printf("resultValue[i]\n");
		showValue(&(selectBody->resultValue[i]));
	}
	printf("%s\n", selectBody->isInner?"in":"out");
	printf("sortColumnName= %s\n", selectBody->sortColumnName);
	switch(selectBody->sortOrder)
	{
	case NOTSORT:
		printf("NOTSORT\n");
		break;
	case DESC:
		printf("DESC\n");
		break;
	case INCR:
		printf("INCR\n");
		break;
	default:
		break;
	}
	printf("condition:\n");
	showCondition(selectBody->condition);
	for (p=selectBody->condition->next; p; p = p->next)
	{
		showCondition(p);
	}
	return 0;
}
int update(UpdateBody *updateBody)
{
	int i;
	printf("function:update\n");
	printf("tableName:%s\n", updateBody->tableName);
	printf("columnAmount = %d\n", updateBody->columnAmount);
	for (i=0;i<updateBody->columnAmount;i++)
	{
		printf("columnsName[i] = %s\n", updateBody->columnsName[i]);
		printf("Values[i]\n");
		showValue(&(updateBody->newValues[i]));
	}
	printf("updatedColumn = %s\n", updateBody->updatedColumn);
	printf("oldvalue:\n");
	showValue(&(updateBody->oldValue));
	return 0;
}
//int delete(char *tableName, Condition *condition);
int delete(char *tableName, char *column, Value value)
{
	printf("function:delete\n");
	printf("tableName:%s\n", tableName);
	printf("column:%s\n", column);
	showValue(&value);
	return 0;
}
int insert(char *tableName, char **columnsName, Value *values, int amount)
{
	int i;
	printf("function:insert\n");
	printf("tableName = %s\n",tableName);
	printf("amount = %d\n", amount);
	for (i=0;i<amount;i++)
	{
		printf("columnsName[i] = %s\n", columnsName[i]);
		showValue(&values[i]);
	}
	return 0;
}

int showDatabase(SORT_ORDER sortOrder)
{
	printf("function: showDatabase\n");
	switch (sortOrder)
	{
	case NOTSORT:
		printf("sortOrder:NOTSORT\n");
		break;
	case DESC:
		printf("sortOrder:DESC\n");
		break;
	case INCR:
		printf("sortOrder:INCR\n");
		break;
	default:
		printf("columnType:unknown\n");
		break;
	}
	return 0;
}
int showTable(char *databaseName,SORT_ORDER sortOrder) //databaseName为NULL时指定为当前数据库
{
	printf("function:showTable\n");
	switch (sortOrder)
	{
	case NOTSORT:
		printf("sortOrder:NOTSORT\n");
		break;
	case DESC:
		printf("sortOrder:DESC\n");
		break;
	case INCR:
		printf("sortOrder:INCR\n");
		break;
	default:
		printf("columnType:unknown\n");
		break;
	}
	return 0;
}
int showColumn(char *tableName, SORT_ORDER sortOrder)
{
	printf("function: showColumn\n");
	switch (sortOrder)
	{
	case NOTSORT:
		printf("sortOrder:NOTSORT\n");
		break;
	case DESC:
		printf("sortOrder:DESC\n");
		break;
	case INCR:
		printf("sortOrder:INCR\n");
		break;
	default:
		printf("columnType:unknown\n");
		break;
	}
	return 0;
}

