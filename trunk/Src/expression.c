#include <STDIO.H>
#include <STDLIB.H>
#include <MATH.H>
#include "expression.h"

SeqList *L[Max];
int kTemp, jTemp = 0;

void Insert(SeqList *L, char a, int i) 
{
	L->List[i] = a;
	if(a == '.') 
	{
		jTemp = i;
		L->List[jTemp] = 48;
	}
	kTemp = i;
}

float Ret(SeqList *L)
{
	int i = 0, a;
	float t = 0;

	if(jTemp == 0)  
		while(i <= kTemp) 
		{
			t = t + (L->List[i] - 48) * (float)(pow(10, kTemp - i)); 
			i++;
		}
	else 
	{
		while(i <= kTemp) 
		{
			if((a = jTemp - i - 1) >= 0) 
				a = jTemp - i - 1;
			else 
				a = jTemp - i;
			t = t + (L->List[i] - 48) * (float)(pow(10, a));
			i++;
		}
	}

	return t;
}

void Makempty2(OSS *s) 
{
	s->bot = 0;
	s->top = -1;
}

void Makempty1(ORS *s) 
{
	s->bot = 0;
	s->top = -1;
}

int Empty2(OSS *s)
{
	if(s->top == -1) 
		return 1; 
	else 
		return 0; 
}

int Empty1(ORS *s)
{
	if(s->top == -1) 
		return 1; 
	else 
		return 0; 
}

char Gettop2(OSS *s)
{
	if(Empty2(s))
	{
		printf("\nOSS empty\n");
		exit(-1);
	}
	else 
		return  s->List1[s->top].a;
}

int Gettop21(OSS *s)
{
	if(Empty2(s))  
	{
		printf("\nOSS empty\n");
		exit(-1);
	}
	else 
		return s->List1[s->top].b ;
}

float Gettop1(ORS *s) 
{
	if(Empty1(s))  
	{
		printf("\nORS empty\n");
		exit(-1);
	}
	else 
		return (s->List[s->top]);
}

void Pop2(OSS *s) 
{
	if(Empty2(s)) 
	{
		printf("\nOSS empty\n");
		exit(-1);
	}
	else 
		s->top--;
}

void Pop1(ORS *s) 
{
	if(Empty1(s)) 
	{
		printf("\nORS empty\n");
		exit(-1);
	}
	else 
		s->top--;
}

void Push2(OSS *s, char x, int y ) 
{
	s->top++;
	s->List1[s->top].a = x;
	s->List1[s->top].b = y;
}

void Push1(ORS *s, float x) 
{
	s->top++;
	s->List[s->top] = x;
}

float Calcul(char *A) 
{
	OSS *q;
	ORS *p;
	char *s = A, c;
	float a, b, f;
	int i = 0, h, w;

	strcat(s, ";");
	q = (OSS *)malloc(sizeof(OSS));
	p = (ORS *)malloc(sizeof(ORS));
	Makempty1 ( p );
	Makempty2 ( q );
	Push2 (q, ';', 0);
	while ( *s != '\0' ) 
	{
		if ( *s==';' || *s=='(' || *s==')' || *s=='-' || *s=='+' || 
			*s=='/' || *s=='*' || *s=='%' || *s=='^' ) 
		{
			switch ( *s ) 
			{
			case ';':
				w = 0;
				break;
			case '(':
			case ')':
				w = 1;
				break;
			case '-':
			case '+':
				w = 2;
				break;
			case '/':
			case '*':
			case '%':
				w = 3;
				break;
			case '^':
				w = 4;
				break;
			}
			if ( *s == '(' || w > Gettop21(q) )  
			{ 
				Push2(q, *s, w);
				*s++;
			} 
			else if (*s == ';' && Gettop2(q) == ';') 
			{
				return (Gettop1(p));
				*s++;
			} 
			else if (*s == ')' && Gettop2(q) == '(' )  
			{ 
				Pop2(q);
				*s++;
			}
			else if ( w <= Gettop21(q)) 
			{
				b = Gettop1(p);
				Pop1(p);
				a = Gettop1(p);
				Pop1(p);
				c = Gettop2(q);
				Pop2(q);
				switch(c) 
				{
				case '+' :
					f = a + b;
					break;
				case '-' :
					f = a - b;
					break;
				case '*' :
					f = a * b;
					break;
				case '/' :
					f = a / b;
					break;
				case '%' :
					f = (float)((int)(a) % (int)(b));
					break;
				case '^' :
					f = (float)pow(a, b);
					break;
				}
				Push1(p, f);
			}
		} 
		else 
		{
			h = 0;
			L[i] = (SeqList *)malloc(sizeof(SeqList));
			while(*s >= 48 && *s <= 57 || *s == 46 )      
			{
				Insert(L[i], *s, h++ );
				*s++;
			}
			Push1(p, Ret(L[i]));
			i++;
		}
	}
}