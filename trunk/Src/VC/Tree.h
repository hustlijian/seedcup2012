/**
 * @file	Tree.h
 * @author  lijian <hustlijian@gmail.com>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * �﷨���������ֵ�ͷ�ļ������������ڵ㣬�������ĺ���
 * ͨ�������﷨�����򻯣�չ����������ʹ������������˳��
 */


#ifndef TREE_H
#define TREE_H
#include "DatabaseAPI.h"

/* ָ���ڵ������ */
typedef enum {LOG,CONDITION,EMP} NODE_TYPE;
/* �ڵ��ֵ */
typedef struct  {
	LOGIC logic; //OR, AND
	Condition *condition;//��������
	NODE_TYPE nodeType;
}NodeValue;
/* �﷨���ڵ� */
typedef struct Node {
	NodeValue nodeValue;
	struct Node *parent;
	struct Node *child;
	struct Node *brother;
} Node;
/* �����ڵ��е�value */
int copyNodeValue(Node *T, NodeValue *value);
/* ���ýڵ�T���ӽڵ㣬�Ҹ�ֵΪvalue */
Node *setChildTree(Node *T, NodeValue *value);
/* ���ýڵ�T�ĺ��ӵ��ֵܽڵ㣬�Ҹ�ֵΪvalue */
Node *setBrotherTree(Node *T, NodeValue *value);
/* �ж��Ƿ��������ڵ� */
int checkNodeCondition(Node *T);
/* ȷ���ڵ�T�ĸ��ڵ��Ƿ���or,and�ڵ� */
int checkNodeLogic(Node *T);
/* 	�����﷨��������һ��condition������ */
void Link(Node *T, Node **tail, Condition **conditon);
/* ȥ���﷨���еĿսڵ� */
void changeTree(Node **T);
/* �����﷨������T����endΪֹ */
Node* copyNode(Node *T, Node *end);
/* �ͷ��﷨��T */
void freeNode(Node *T);
/* չ���﷨���е�����*/
void cutTree(Node **T);
/* 	����﷨������������ */
void PrintBiTree(Node *T, int depth);
#endif
