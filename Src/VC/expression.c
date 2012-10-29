 /**
 * @file    Expression.c
 * @author  ChenPei <xingkongcp@gmail.com>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * ������ѧ���ʽ����
 */
 
#include <STDIO.H>
#include <STDLIB.H>
#include <STRING.H>
#include <MATH.H>
#include <CTYPE.H>
#include "expression.h"

SeqList *L[MAX_DEPTH];
int kTemp, jTemp = 0;

/**
 * <����ֵ���в�����ֵ>
 *
 * @param   L ��ֵ��ṹ��
 * @param	a ��ֵ��'.'
 * @param	i ����λ��
 * @return  NULL
 */
void Insert(SeqList *L, char a, int i)
{
	L->List[i] = a;
	if(a == '.')
	{
		jTemp = i;
		L->List[jTemp] = 48;	//0��ASCII��
	}
	kTemp = i;
}

/**
 * <����ֵ���еĲ���������>
 *
 * @param   L ��ֵ��ṹ��
 * @return  ������
 */
float Ret(SeqList *L)
{
	int i = 0, a;
	float t = 0;

	if(jTemp == 0)
		while(i <= kTemp)
		{
			t = t + (L->List[i] - '0') * (float)(pow(10, kTemp - i));
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

/**
 * <��ղ�����ջ>
 *
 * @param   s ������ջ�ṹ��
 * @return  NULL
 */
void Makempty2(OSS *s)
{
	s->bot = 0;
	s->top = -1;
}

/**
 * <��ղ�����ջ>
 *
 * @param   s ������ջ�ṹ��
 * @return  NULL
 */
void Makempty1(ORS *s)
{
	s->bot = 0;
	s->top = -1;
}

/**
 * <�жϲ�����ջ�Ƿ�Ϊ��>
 *
 * @param   s ������ջ�ṹ��
 * @return  0��ʾΪ�գ�-1��ʾ��Ϊ��
 */
int isOSSEmpty(OSS *s)
{
	if(s->top == -1)
		return 0;
	else
		return -1;
}

/**
 * <�жϲ�����ջ�Ƿ�Ϊ��>
 *
 * @param   s ������ջ�ṹ��
 * @return  0��ʾΪ�գ�-1��ʾ��Ϊ��
 */
int isORSEmpty(ORS *s)
{
	if(s->top == -1)
		return 0;
	else
		return -1;
}

/**
 * <��ȡ����������Ԫ��>
 *
 * @param   s ������ջ�ṹ��
 * @param	ch ������ջ��Ԫ��
 * @return  0��ʾ��ȡ�ɹ���-1��ʾ��ȡʧ��
 */
int Gettop2(OSS *s, char *ch)
{
	if(!isOSSEmpty(s))
	{
		return -1;
	}
	else
	{
		*ch = s->List1[s->top].a;
		return 0;
	}
}

/**
 * <��ȡ������Ԫ�����ȼ�>
 *
 * @param   s ������ջ�ṹ��
 * @param	i ������ջ��Ԫ�����ȼ�
 * @return  0��ʾ��ȡ�ɹ���-1��ʾ��ȡʧ��
 */
int Gettop21(OSS *s, int *i)
{
	if(!isOSSEmpty(s))
	{
		return -1;
	}
	else
	{
		*i = s->List1[s->top].b ;
		return 0;
	}
}

/**
 * <��ȡ������ջ��Ԫ��>
 *
 * @param   s ������ջ�ṹ��
 * @param	i ������ջ��Ԫ��
 * @return  0��ʾ��ȡ�ɹ���-1��ʾ��ȡʧ��
 */
int Gettop1(ORS *s, float *flt)
{
	if(!isORSEmpty(s))
	{
		return -1;
	}
	else
	{
		*flt = (s->List[s->top]);
		return 0;
	}
}

/**
 * <����������Ԫ��>
 *
 * @param   s �������ṹ��
 * @return  0��ʾ�����ɹ���-1��ʾ����ʧ��
 */
int Pop2(OSS *s)
{
	if(!isOSSEmpty(s))
	{
		return -1;
	}
	else
	{
		s->top--;
		return 0;
	}
}

/**
 * <����������Ԫ��>
 *
 * @param   s �������ṹ��
 * @return  0��ʾ�����ɹ���-1��ʾ����ʧ��
 */
int Pop1(ORS *s)
{
	if(!isORSEmpty(s))
	{
		return -1;
	}
	else
	{
		s->top--;
		return 0;
	}
}

/**
 * <ѹ�������Ԫ��>
 *
 * @param   s �������ṹ��
 * @param	x ������
 * @param	y ���������ȼ�
 * @return  NULL
 */
void Push2(OSS *s, char x, int y ) 
{
	s->top++;
	s->List1[s->top].a = x;
	s->List1[s->top].b = y;
}

/**
 * <ѹ�������Ԫ��>
 *
 * @param   s �������ṹ��
 * @param	x ������
 * @return  NULL
 */
void Push1(ORS *s, float x)
{
	s->top++;
	s->List[s->top] = x;
}

/**
 * <������ѧ���ʽ>
 *
 * <���������'(', ')', '+', '-', '*'
 *	'/', '%', '^'>
 * @param   A ���ʽ
 * @param	result ������
 * @return  0��ʾ����ɹ���-1��ʾ����ʧ��
 */
int calExpression(char *A, float *result)
{
	OSS *q;
	ORS *p;
	char *s = A, c, chTemp;
	float a, b, f, fTemp;
	int i = 0, h, w, iTemp;
	int negFlag = 0;

	strcat(s, ";");
	q = (OSS *)malloc(sizeof(OSS));
	p = (ORS *)malloc(sizeof(ORS));
	Makempty1 ( p );
	Makempty2 ( q );
	Push2 (q, ';', 0);
	while ( *s != '\0' )
	{
		//��������֮��ķ���
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
				s++;
			}
			else if (*s == ';' && chTemp == ';')
			{
				if (Gettop1(p, &fTemp))
					return -1;
				*result = fTemp;
				s++;
			}
			else if (*s == ')' && chTemp == '(' )
			{
				if(Pop2(q))
					return -1;
				s++;
			}
			else if ( w <= iTemp && (isdigit(*(s-1)) || *(s-1) == '(' || *(s-1) == ')'))//
			{
				if (Gettop1(p, &fTemp))
					return -1;
				else
				{
					b = fTemp;
					if(Pop1(p))
						return -1;
				}
				if (Gettop1(p, &fTemp))	//������
					a = 0;//return -1;
				else
				{
					a = fTemp;
					if(Pop1(p))
						return -1;
				}
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
					if((int)a != a)		//a����Ϊ������
						return -1;
					if(!b)				//b����Ϊ0
						return -1;
					f = (float)((int)(a) % (int)(b));
					break;
				case '^' :
					f = (float)pow(a, b);
					break;
				default:
					return -1;
				}
				c = '\0';
				Push1(p, f);
			}
			else
			{
				Push2(q, *s, w);
				s++;
			}
		}
		else
		{
			h = 0;
			L[i] = (SeqList *)malloc(sizeof(SeqList));
			if (*(s-1) == '+' && !isdigit(*(s-2)) && *(s-2) != ')')
			{
				Pop2(q);
			}
			else if (*(s-1) == '-' && !isdigit(*(s-2)) && *(s-2) != ')')
			{
				Pop2(q);
				negFlag = 1;
			}
			while(isdigit(*s) || *s == '.' )
			{
				Insert(L[i], *s, h++ );
				s++;
			}
			if (negFlag == 1)			//-8��������
			{
				Push1(p, 0-Ret(L[i]));	
				negFlag = 0;
			}
			else
				Push1(p, Ret(L[i]));
			i++;
		}
	}

	if (p->top != 0)					//ȱ������������
		return -1;

	return 0;
}
