#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "CmdProcess.h"

char keywords[NUM_KEYWORDS][MAX_KEYWORDS_LENGTH]={
	"create", "database","table", "alter","truncate","add","column","use",
		"drop","rename","select","from","where","order","by", "desc", "incr", 
		"update","set","delete", "insert", "into", "values", "show", 
		"databases","int","float","text", "none","between","like","and", "or"
};
char operatorwords[NUM_OPERATOR][MAX_OPERATOR] = { 
	"(", ")","*", "/", "%","+", "-", "==", "~=", ">=", "<=", ">", "<","[",
		"]",";",",","'","?"
};
/*功能：
 *     不区分大小写的字符比较
 * 参数：
 * 
*/
int mystrcmp(const char *str1, const char *str2)
{
	int i=0;
	char c1, c2;
	do 
	{
		c1=tolower(str1[i]);
		c2=tolower(str2[i++]);
	} while ((c1==c2)&&c1);
	return (c1-c2);
}

/* 功能：
 *		获得string的类型
 * 参数：
 * @str:  没有空格的字符串
 * 返回：
 *		整形，类型码,其中0代表不正确的输入字符
 */
int getTypeNum(char *str)
{
	int i, flag;
	
	for (i=0, flag=0;str[i]&&(isdigit(str[i])||str[i]=='.');i++)
		if (str[i]=='.'){
			flag++;
		}
	if (str[i]=='\0')//数字
	{
		if (flag==1) return 42; //浮点数
		if (flag>1) return 0;//错误输入
		return 41; //整数
	} 
	if (isalpha(str[0])||str[0]=='_') //标识符
	{
		for (i=0;i<NUM_KEYWORDS;i++)
			if (!mystrcmp(keywords[i], str))
				break;
		if (i==NUM_KEYWORDS) return 40;//一般标识符
		else return (i+1);
	}
	//运算符
	for (i=0;i<NUM_OPERATOR;i++)
		if (!strcmp(str, operatorwords[i]))
			break;
	if (i<NUM_OPERATOR) return (i+50);
	else return 0;
	return 0;
}
/*
 * function: scaner the str, and store one words to word
 * parameters:
 *		@str: the input string
 *		@word: store the result word
 *		@sys: the type of the word
 *		@wordLenght: the length of word
 * return:
 *		next position of the string 
 */
char *scaner(char *str, char *word, int *syn)
{
	int i, j;
	char ch, temp;

	for (i=0; i<MAX_WORDLEGTH; i++)
		word[i] = '\0';

	i = 0;
	while((isspace(ch=str[i++]))) ; /* skip the space*/
	if (ch&&(isalnum(ch)||ch=='_'||ch=='.'))  /* words or digits */
	{
		j = 0;
		while(ch&&(isalnum(ch)||ch=='_'||ch=='.')) {
			word[j++] = ch;
			ch = str[i++];
		}
		word[j] = '\0';
		(*syn) = getTypeNum(word); /* get the type number */
	}
	else if(ch){
		j = 0;
		word[j++] = ch;
		temp = ch;
		ch = str[i++];
		switch (temp) {
			case '=':
				if (ch!='=')
					break;
				//fall through
			case '~': 
				if (ch!='=')
					break;
				//fall through
			case '<':
				if (ch != '=')
					break;
				//fall through
			case '>':
				if (ch != '=') 
					break;

				word[j++] = ch;				
				i++;				
				break;
			default:
				break;
		}
		word[j] = '\0';
		(*syn) = getTypeNum(word); /* get the type number */
	}
	if(ch) return (str+i-1);   /* return the position */
	else return NULL;
}

/* 处理一条命令cmd */
int processCmd(char *cmd)
{
	char *p=cmd;
	char word[MAX_WORDLEGTH];
	int syn;
	do {
		p = scaner(p, word, &syn);
		if(word[0]) printf("(%s, %d)\n", word, syn);
	} while (syn&&(p!=NULL));
	return 0;
}
/*用来测试的程序*/
void test()
{

}
