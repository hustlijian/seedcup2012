#ifndef _EXPRESSION_H
#define _EXPRESSION_H

#define Max 20

typedef struct
{
	float List[Max];
	int top, bot;
}ORS;

typedef struct
{
	char a;
	int b;
}Dype;

typedef struct
{
	Dype List1[Max];
	int top, bot;
}OSS;

typedef struct
{
	char List[Max];
	int n;
}SeqList;

int Calcul(char *A, float *result);

#endif	//_EXPRESSION_H