#ifndef TREE_H
#define TREE_H
#include "DatabaseAPI.h"

typedef enum {LOG,CONDITION,EMP} NODE_TYPE;
typedef struct  {
	LOGIC logic; //OR, AND
	Condition *condition;//±£´æÌõ¼þ
	NODE_TYPE nodeType;
}NodeValue;

typedef struct Node {
	NodeValue nodeValue;
	struct Node *parent;
	struct Node *child;
	struct Node *brother;
} Node;

int copyNodeValue(Node *T, NodeValue *value);
Node *setChildTree(Node *T, NodeValue *value);
Node *setBrotherTree(Node *T, NodeValue *value);
int checkNodeCondition(Node *T);
int checkNodeLogic(Node *T);
void Link(Node *T, Node **tail, Condition **conditon);
void changeTree(Node **T);
Node* copyNode(Node *T, Node *end);
void freeNode(Node *T);
void cutTree(Node **T);
void PrintBiTree(Node *T, int depth);
#endif
