/**
 * @file	main.c
 * @author  lijian <hustlijian@gmail.com>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * 读取一行命令，以分号结尾，调用一行命令的解析函数，根据返回值报错
 */


#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "DatabaseAPI.h"
#include "CmdProcess.h"

int main(int argc, char* argv[])
{	
	char str[1024], c, c2;
	int i = 0, j;

	if (argc < 3)
	{
		printf("check your argument!\n");
		return -1;
	}


	if(freopen(argv[1], "r", stdin)==NULL)
	{
		printf("can not open %s\n", argv[1]);
		return -1;
	}
	freopen(argv[2],"w", stdout);

	do 
	{
		for (i=0,c2=c=getchar();c!=';'&&c!=EOF;) {
			str[i++]=c;			
			if (c2=='/' && c=='/' && i>1)//行注释
			{
				i-= 2;
				str[i] = '\0';
				while((c=getchar())!='\n')
					if (c == EOF)
						break;
			}
			if (c2 == '/' && c == '*')
			{
				i-= 2;
				str[i] = '\0';
				while(c!=EOF)
				{
					c = getchar();
					if (c2 == '*' && c == '/')
					{
						break;
					}
					c2 = c;
				}
			}
			c2=c;
			if (c != EOF)
				c=getchar();
		}
		if (c==';')
			str[i]=';';
		else str[i]='\0';
		str[i+1] = '\0';
		if (c == EOF) {
			for(j=0;isspace(str[j]);j++);
			if (i!=j) printf("error\n");
			break;
		}
		if(processCmd(str)) //返回为非零报错
			printf("error\n");
	} while (1);
	
	return 0;
}