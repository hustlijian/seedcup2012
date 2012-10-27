#include <STDIO.H>
#include <STDLIB.H>
#include <STRING.H>
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
		return 0; 
	else 
		return -1; 
}

int Empty1(ORS *s)
{
	if(s->top == -1) 
		return 0; 
	else 
		return -1; 
}

char Gettop2(OSS *s, char *ch)
{
	if(!Empty2(s))
	{
		//printf("\nOSS empty\n");
		return -1;
	}
	else 
	{
		*ch = s->List1[s->top].a;
		return 0;
	}
}

int Gettop21(OSS *s, int *i)
{
	if(!Empty2(s))  
	{
		//printf("\nOSS empty\n");
		return -1;
	}
	else 
	{
		*i = s->List1[s->top].b ;
		return 0;
	}
}

int Gettop1(ORS *s, float *flt) 
{
	if(!Empty1(s))  
	{
		//printf("\nORS empty\n");
		return -1;
	}
	else 
	{
		*flt = (s->List[s->top]);
		return 0;
	}
}

int Pop2(OSS *s) 
{
	if(!Empty2(s)) 
	{
		//printf("\nOSS empty\n");
		return -1;
	}
	else 
	{
		s->top--;
		return 0;
	}
}

int Pop1(ORS *s) 
{
	if(!Empty1(s)) 
	{
		//printf("\nORS empty\n");
		return -1;
	}
	else 
	{
		s->top--;
		return 0;
	}
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

int calExpression(char *A, float *result) 
{
	OSS *q;
	ORS *p;
	char *s = A, c, chTemp;
	float a, b, f, fTemp;
	int i = 0, h, w, iTemp;

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
			if (Gettop21(q, &iTemp))
				return -1;
			if (Gettop2(q, &chTemp))
				return -1;
			if ( *s == '(' || w > iTemp )  
			{ 
				Push2(q, *s, w);
				*s++;
			} 
			else if (*s == ';' && chTemp == ';') 
			{
				if (Gettop1(p, &fTemp))
					return -1;
				*result = fTemp;
				*s++;
			} 
			else if (*s == ')' && chTemp == '(' )  
			{ 
				if(Pop2(q))
					return -1;
				*s++;
			}
			else if ( w <= iTemp) 
			{
				if (Gettop1(p, &fTemp))
					return -1;
				b = fTemp;
				if(Pop1(p))
					return -1;
				if (Gettop1(p, &fTemp))
					return -1;
				a = fTemp;
				if(Pop1(p))
					return -1;
				if (Gettop2(q, &chTemp))
					return -1;
				c = chTemp;
				if(Pop2(q))
					return -1;
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
					if (!b)
						return -1;
					f = a / b;
					break;
				case '%' :
					if(!b)
						return -1;
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

	return 0;
}