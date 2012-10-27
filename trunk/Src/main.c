#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "DatabaseAPI.h"
#include "CmdProcess.h"

int main(int argc, char* argv[])
{	
	char str[1024], c, c2;
	char inputFile[64]="";
	char outputFile[64]="";
	int i = 0, j;

	if (argc < 3)
	{
		printf("check your argument!\n");
		return -1;
	}

	do 
	{
		i++;
		strcat(inputFile, argv[i]);
		strcat(inputFile, " ");
	} while (strcmp(&argv[i][strlen(argv[i])-4] , ".txt"));

	do 
	{
		i++;
		strcat(outputFile, argv[i]);
		strcat(outputFile, " ");
	} while (strcmp(&argv[i][strlen(argv[i])-4] , ".txt"));

	if(freopen(inputFile, "r", stdin)==NULL)
	{
		printf("can not open %s\n", inputFile);
		return -1;
	}
	freopen(outputFile,"w", stdout);

	do 
	{
		for (i=0,c2=c=getchar();c!=';'&&c!=EOF;) {
			str[i++]=c;			
			if (c2=='/' && c=='/' && i>1)
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
				while(1)
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