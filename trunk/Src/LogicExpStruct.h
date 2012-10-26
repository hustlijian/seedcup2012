#ifndef _LOGIC_EXP_STRUCT_H
#define _LOGIC_EXP_STRUCT_H

/*#include "DatabaseAPI.h"*/

#define STACK_INIT_SIZE 10
#define STACK_INCR_SIZE 10

typedef struct node
{
	char data[20];
	int w;
	struct node *next;
}node;

typedef struct
{
	int length;
	node *SP;
}LogicExpStack;

int logicalExpStkInit(LogicExpStack *stk);
int logicalExpStkGetTop(const LogicExpStack *stk, char **var, int *w);
int logicalExpStkPush(LogicExpStack *stk, char *var, int w);
int logicalExpStkPop(LogicExpStack *stk, char **var, int *w);
int logicalExpStkEmpty(const LogicExpStack *stk);
/*int logicalExpListInsert(Condition **List, char *cName, Value val1, Value val2, OPERATOR opt);*/

#endif	//_LOGIC_EXP_STRUCT_H