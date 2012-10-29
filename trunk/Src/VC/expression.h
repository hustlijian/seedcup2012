 /**
 * @file    Expression.h
 * @author  ChenPei <xingkongcp@gmail.com>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * ������ѧ���ʽ�������ݽṹ
 */
 
#ifndef _EXPRESSION_H
#define _EXPRESSION_H

#define MAX_DEPTH 20		//��ֵ����󳤶�

/**
 * <������ջ>
 */
typedef struct
{
	float List[MAX_DEPTH];
	int top, bot;
}ORS;

/**
 * <�������������ȼ�>
 */
typedef struct
{
	char a;
	int b;
}Dype;

/**
 * <������ջ>
 */
typedef struct
{
	Dype List1[MAX_DEPTH];
	int top, bot;
}OSS;

/**
 * <��ֵ��>
 */
typedef struct
{
	char List[MAX_DEPTH];
	int n;
}SeqList;

/**
 * <���㸴����ѧ���ʽ>
 */
int calExpression(char *A, float *result);

#endif	//_EXPRESSION_H