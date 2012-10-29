 /**
 * @file    Expression.c
 * @author  ChenPei <xingkongcp@gmail.com>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * 复杂数学表达式处理
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
 * <向数值表中插入数值>
 *
 * @param   L 数值表结构体
 * @param	a 数值或'.'
 * @param	i 插入位置
 * @return  NULL
 */
void Insert(SeqList *L, char a, int i)
{
	L->List[i] = a;
	if(a == '.')
	{
		jTemp = i;
		L->List[jTemp] = 48;	//0的ASCII码
	}
	kTemp = i;
}

/**
 * <将数值表中的操作数读出>
 *
 * @param   L 数值表结构体
 * @return  操作数
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
 * <清空操作符栈>
 *
 * @param   s 操作符栈结构体
 * @return  NULL
 */
void Makempty2(OSS *s)
{
	s->bot = 0;
	s->top = -1;
}

/**
 * <清空操作数栈>
 *
 * @param   s 操作数栈结构体
 * @return  NULL
 */
void Makempty1(ORS *s)
{
	s->bot = 0;
	s->top = -1;
}

/**
 * <判断操作符栈是否为空>
 *
 * @param   s 操作符栈结构体
 * @return  0表示为空，-1表示不为空
 */
int isOSSEmpty(OSS *s)
{
	if(s->top == -1)
		return 0;
	else
		return -1;
}

/**
 * <判断操作数栈是否为空>
 *
 * @param   s 操作数栈结构体
 * @return  0表示为空，-1表示不为空
 */
int isORSEmpty(ORS *s)
{
	if(s->top == -1)
		return 0;
	else
		return -1;
}

/**
 * <获取操作数符顶元素>
 *
 * @param   s 操作符栈结构体
 * @param	ch 操作符栈顶元素
 * @return  0表示获取成功，-1表示获取失败
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
 * <获取操作符元素优先级>
 *
 * @param   s 操作符栈结构体
 * @param	i 操作符栈顶元素优先级
 * @return  0表示获取成功，-1表示获取失败
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
 * <获取操作数栈顶元素>
 *
 * @param   s 操作数栈结构体
 * @param	i 操作数栈顶元素
 * @return  0表示获取成功，-1表示获取失败
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
 * <弹出操作符元素>
 *
 * @param   s 操作符结构体
 * @return  0表示弹出成功，-1表示弹出失败
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
 * <弹出操作数元素>
 *
 * @param   s 操作数结构体
 * @return  0表示弹出成功，-1表示弹出失败
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
 * <压入操作符元素>
 *
 * @param   s 操作符结构体
 * @param	x 操作符
 * @param	y 操作符优先级
 * @return  NULL
 */
void Push2(OSS *s, char x, int y ) 
{
	s->top++;
	s->List1[s->top].a = x;
	s->List1[s->top].b = y;
}

/**
 * <压入操作数元素>
 *
 * @param   s 操作数结构体
 * @param	x 操作数
 * @return  NULL
 */
void Push1(ORS *s, float x)
{
	s->top++;
	s->List[s->top] = x;
}

/**
 * <计算数学表达式>
 *
 * <运算符包括'(', ')', '+', '-', '*'
 *	'/', '%', '^'>
 * @param   A 表达式
 * @param	result 计算结果
 * @return  0表示计算成功，-1表示计算失败
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
		//除操作数之外的符号
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
				if (Gettop1(p, &fTemp))	//隐患处
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
					if((int)a != a)		//a不能为非整数
						return -1;
					if(!b)				//b不能为0
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
			if (negFlag == 1)			//-8这类数据
			{
				Push1(p, 0-Ret(L[i]));	
				negFlag = 0;
			}
			else
				Push1(p, Ret(L[i]));
			i++;
		}
	}

	if (p->top != 0)					//缺少运算符的情况
		return -1;

	return 0;
}
