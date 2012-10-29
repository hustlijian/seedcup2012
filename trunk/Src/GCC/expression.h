 /**
 * @file    Expression.h
 * @author  ChenPei <xingkongcp@gmail.com>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * 复杂数学表达式所需数据结构
 */
 
#ifndef _EXPRESSION_H
#define _EXPRESSION_H

#define MAX_DEPTH 20		//数值表最大长度

/**
 * <操作数栈>
 */
typedef struct
{
	float List[MAX_DEPTH];
	int top, bot;
}ORS;

/**
 * <操作符及其优先级>
 */
typedef struct
{
	char a;
	int b;
}Dype;

/**
 * <操作符栈>
 */
typedef struct
{
	Dype List1[MAX_DEPTH];
	int top, bot;
}OSS;

/**
 * <数值表>
 */
typedef struct
{
	char List[MAX_DEPTH];
	int n;
}SeqList;

/**
 * <计算复杂数学表达式>
 */
int calExpression(char *A, float *result);

#endif	//_EXPRESSION_H