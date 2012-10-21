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

/*
 *Parameters:
 *	@str1:
 *	@str2:
 *Return:
 *	
 *Description:
 *	Compare two strings
 *Date:
 *	2012-10-21 10:58:18
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

/*
 *Parameters:
 * @str:string without spaces
 *Return:
 *	0:incorrect input character
 *	else:type number
 *Description:
 *	Get the type number of the string
 *Date:
 *	2012-10-21 10:49:31
 */
int getTypeNum(char *str)
{
	int i, flag;
	
	for (i=0, flag=0; str[i] && (isdigit(str[i]) || str[i]=='.'); i++)
		if (str[i]=='.'){
			flag++;
		}
	/* number */
	if (str[i]=='\0')
	{
		/* float */
		if (flag==1) return 42; 
		/* wrong */
		if (flag>1) return 0;
		/* integer */
		return 41; 
	} 
	/* identifier */
	if (isalpha(str[0])||str[0]=='_') 
	{
		for (i=0;i<NUM_KEYWORDS;i++)
			if (!mystrcmp(keywords[i], str))
				break;
		/* normal identifier */
		if (i==NUM_KEYWORDS) return 40;
		else return (i+1);
	}
	/* operator */
	for (i=0;i<NUM_OPERATOR;i++)
		if (!strcmp(str, operatorwords[i]))
			break;
	if (i<NUM_OPERATOR) return (i+50);
	else return 0;
	return 0;
}

/*
 *Parameters:
 *	@str: the input string
 *	@word: store the result word
 *	@sys: the type of the word
 *	@wordLenght: the length of word
 *Return:
 *	Next position of the string 
 *Description: 
 *	Scaner the str, and store one words to word
 *Date:
 *	2012-10-21 10:44:58
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

/*
 *Parameters:
 *	@cmd:one command(ended by ';')
 *Return:
 *	0:success
 *	-1:failed
 *Description:
 *	Deal with one command.
 *Date:
 *	2012-10-21 10:42:04
 */
int processCmd(char *cmd)
{
	char *p = cmd;
	char word[MAX_WORDLEGTH];
	int syn;

	do {
		p = scaner(p, word, &syn);
		if(word[0]) 
			printf("(%s, %d)\n", word, syn);
	} while (syn && (p != NULL));

	return 0;
}
