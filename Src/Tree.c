#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Tree.h"
#include "DatabaseAPI.h"

int copyValue(Value *a,const Value *b)
{
	a->columnType = b->columnType;
	switch(b->columnType)
	{
	case INT:
		a->columnValue.intValue = b->columnValue.intValue;
		break;
	case FLOAT:
		a->columnValue.floatValue = b->columnValue.floatValue;
		break;
	case TEXT:
		a->columnValue.textValue = (char *)malloc(sizeof(char)*NAME_MAX);
		strcpy(a->columnValue.textValue, b->columnValue.textValue);
		break;
	default:
		break;
	}
	return 0;
}
int copyNodeValue(Node *T, NodeValue *value)
{
	Condition *con;
	T->nodeValue.nodeType = value->nodeType;
	switch(value->nodeType)
	{
	case  LOG:		
		T->nodeValue.logic = value->logic;
		break;
	case CONDITION:
		//T->nodeValue.condition = value->condition;
		con = (Condition *)malloc(sizeof(Condition));
		con->next = NULL;
		con->operator = value->condition->operator;
		con->logic = value->condition->logic;
		copyValue(&(con->value), &(value->condition->value));
		copyValue(&(con->value2), &(value->condition->value2));
		strcpy(con->columnName, value->condition->columnName);
		T->nodeValue.condition =con;
		break;
	default:
		break;
	}
	return 0;
}
Node *setChildTree(Node *T, NodeValue *value)
{
	Node *temp;
	temp = (Node *)malloc(sizeof(Node));
	temp->child = temp->brother = NULL;
	temp->parent = T;
	if (copyNodeValue(temp, value))
		return NULL;		
	T->child = temp;
	return temp;
}

Node *setBrotherTree(Node *T, NodeValue *value)
{
	Node *temp, *temp2;
	temp = (Node *)malloc(sizeof(Node));
	if (copyNodeValue(temp, value))
		return NULL;
	temp->brother = temp->child = NULL;
	temp2 = T->child;
	while (temp2->brother != NULL)
	{
		temp2 = temp2->brother;
	}
	temp2->brother = temp;
	temp->parent = T;
	return temp;
}

//是叶子
int checkNodeCondition(Node *T)
{
	if (T->nodeValue.nodeType == CONDITION)
		return 1;
	return 0;
}
int checkNodeLogic(Node *T)
{//T->parent && T->brother
	if (T->parent==NULL || T->brother==NULL)
		return 0;

	if (T->parent->nodeValue.nodeType == LOG)
		return 1;
	return 0;
}

void Link(Node *T, Node **tail, Condition **conditon)
{
	if (T==NULL) return ;
	Link(T->child, tail, conditon);
	if (checkNodeCondition(T)) {
		if ((*tail) == NULL)
		{
			(*tail) = T;
			(*conditon) = (T->nodeValue.condition);
		} else {
			(*tail)->nodeValue.condition->next = (T->nodeValue.condition);
			(*tail) = T;
		}
	}
	if(checkNodeLogic(T)){
		(*tail)->nodeValue.condition->logic = T->parent->nodeValue.logic;
	}
	Link(T->brother, tail, conditon);
}
void changeTree(Node **T)
{
	Node *p;
	Node *prior;
	if (*T == NULL)
		return ;
	p = *T;
	changeTree(&(p->child));
	changeTree(&(p->brother));
	if ((p->nodeValue.nodeType==EMP))
	{
		if ((*T)->parent == NULL)
			*T = p->child;
		else {
			if (p->parent->child == p)
			{
				prior = NULL;
			} else {
				for (prior=p->parent->child; prior->brother != p; prior = prior->brother)
					;
			}
			if (prior != NULL)
				prior->brother = p->child;
			else
				p->parent->child = p->child;
			p->child->brother = p->brother;
			p->child->parent = p->parent;
			free(p);
		}
	}
}

Node* copyNode(Node *T, Node *end)
{
	Node *temp;
	if (T==end || T==NULL)
		return NULL;
	temp = (Node *)malloc(sizeof(Node));
	if (temp==NULL) return NULL; //error
	copyNodeValue(temp, &T->nodeValue);
	temp->parent = T->parent;
	temp->child = copyNode(T->child, end);
	temp->brother = copyNode(T->brother, end);
	return temp;
}
void freeNode(Node *T)
{
	if (T==NULL)
		return ;
	freeNode(T->child);
	freeNode(T->brother);
	freeNode(T);
}
void cutTree(Node **T)
{
	Node *p = *T;
	Node *q, *c, *c2, *a, *e;
	NodeValue value;
	if (*T == NULL)
		return ;

	cutTree(&(p->child));
	cutTree(&(p->brother));
	if(p->nodeValue.nodeType == LOG)
	{
		q = p->child;
		while(q!=NULL) {
			if (q->nodeValue.nodeType == LOG)
				break;
			q=q->brother;
		}
		if (q==NULL)
			return;
		if (p->nodeValue.logic == AND && q->nodeValue.logic == OR)
		{
			//去括号
			if (p->parent == NULL || p->parent->nodeValue.nodeType == EMP)
			{//添加一个+
				p->parent = (Node*)malloc(sizeof(Node));
				p->parent->nodeValue.nodeType = LOG;
				p->parent->nodeValue.logic = OR;
				p->parent->child = p;
				p->parent->parent = p->parent->brother = NULL;
				(*T) = p->parent;
			}
			c = q->child;
			while(c->brother != NULL){
				c2 = c->brother;
				//添加一个*
				value.nodeType = LOG;
				value.logic = AND;
				a = setBrotherTree(p->parent, &value);
				a->child = copyNode(p->child, q);
				if (a->child == NULL) {
					a->child = copyNode(c,c2);
					a->child->brother = copyNode(q->brother, NULL);
					a->child->parent = a;
				} else {
					for (e=a->child;e->brother!=NULL;e=e->brother)
						;
					e->brother = copyNode(c, c2);
					e->brother->brother = copyNode(q->brother, NULL);
					e->brother->parent = a;
				}
				c->brother = copyNode(q->brother, NULL);
				c->parent = a;
				c = c2;
			}
			//b取代+
			c->brother = q->brother;
			c->parent = p;
			if (p->child == q)
			{
				p->child = c;
			} else {
				for (c2=p->child; c2->brother!=q; c2=c2->brother)
					;
				c2->brother = c;
			}
			cutTree(&(p));
		}
	}
	cutTree(&(p->brother));
}

void PrintBiTree(Node *T, int depth)
{
	int i;
	if (T == NULL) return ;
	
	for (i=0; i<depth; i++) {
		if (i+1 == depth) {
			printf("├─");
			break;
		}
		printf("│  ");
		
	}
	switch(T->nodeValue.nodeType)
	{
	case LOG:
		if (T->nodeValue.logic == OR)
			printf("+\n");
		else
			printf("*\n");
		break;
	case CONDITION:
		printf("%s\n", T->nodeValue.condition->columnName);
		break;
	case EMP:
		printf("T\n");
		break;
	default:
		printf("#\n");
		break;
	}
	
	if (T->child != NULL) {
		PrintBiTree(T->child, depth+1);
	}
	if (T->brother != NULL) {
		PrintBiTree(T->brother, depth);
	}
	
	return ;
}