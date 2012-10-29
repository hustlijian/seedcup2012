/**
 * @file	Tree.h
 * @author  lijian <hustlijian@gmail.com>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * 语法分析树部分的头文件，定义了树节点，树操作的函数
 * 通过建立语法树，简化，展开，链化，使复杂条件运算顺利
 */


#ifndef TREE_H
#define TREE_H
#include "DatabaseAPI.h"

/* 指定节点的类型 */
typedef enum {LOG,CONDITION,EMP} NODE_TYPE;
/* 节点的值 */
typedef struct  {
	LOGIC logic; //OR, AND
	Condition *condition;//保存条件
	NODE_TYPE nodeType;
}NodeValue;
/* 语法树节点 */
typedef struct Node {
	NodeValue nodeValue;
	struct Node *parent;
	struct Node *child;
	struct Node *brother;
} Node;
/* 拷贝节点中的value */
int copyNodeValue(Node *T, NodeValue *value);
/* 设置节点T的子节点，且赋值为value */
Node *setChildTree(Node *T, NodeValue *value);
/* 设置节点T的孩子的兄弟节点，且赋值为value */
Node *setBrotherTree(Node *T, NodeValue *value);
/* 判断是否是条件节点 */
int checkNodeCondition(Node *T);
/* 确定节点T的父节点是否是or,and节点 */
int checkNodeLogic(Node *T);
/* 	遍历语法树，生成一个condition的链表 */
void Link(Node *T, Node **tail, Condition **conditon);
/* 去除语法树中的空节点 */
void changeTree(Node **T);
/* 拷贝语法树，从T，到end为止 */
Node* copyNode(Node *T, Node *end);
/* 释放语法树T */
void freeNode(Node *T);
/* 展开语法树中的括号*/
void cutTree(Node **T);
/* 	输出语法树，用来调试 */
void PrintBiTree(Node *T, int depth);
#endif
