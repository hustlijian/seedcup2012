#include <STDLIB.H>
#include <string.h>
#include "LogicExpStruct.h"

int logicalExpStkInit(LogicExpStack *stk)
{
	node *cur;
	cur = (node *)malloc(sizeof(node));
	//cur->data = NULL;
	cur->next = NULL;
	stk->length = 1;
	stk->SP = cur;
	return 0;
}

int logicalExpStkGetTop(const LogicExpStack *stk, char **var, int *w)
{
	if (!logicalExpStkEmpty(stk))
		return -1;
	*var = (stk->SP->next->data);
	*w = stk->SP->next->w;
	return 0;
}

int logicalExpStkPush(LogicExpStack *stk, char *var, int w)
{
	node *cur;
	
	strcpy(stk->SP->data, var);
	stk->SP->w = w;
	cur = (node *)malloc(sizeof(node));
	//cur->next->data = NULL;
	cur->next = stk->SP;
	stk->SP = cur;
	stk->length++;
	
	return 0;
}

int logicalExpStkPop(LogicExpStack *stk, char **var, int *w)
{
	node *cur;
	
	if (!logicalExpStkEmpty(stk))
		return -1;

	cur = stk->SP;
	stk->SP = stk->SP->next;
	//var = stk->SP->data;
	*var = stk->SP->data;
	*w = stk->SP->w;
	stk->length--;
	free(cur);

	return 0;
}


int logicalExpStkEmpty(const LogicExpStack *stk)
{
	if(stk->length == 1)
		return 0;
	return -1;
}

/*
int logicalExpListInsert(
	Condition **List, 
	char *cName,
	Value val1,
	Value val2,
	OPERATOR opt)
{
	return 0;
}*/