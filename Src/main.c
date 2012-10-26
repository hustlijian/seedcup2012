#include <stdio.h>
#include <string.h>
#include "DatabaseAPI.h"
#include "CmdProcess.h"

int main(int argc, char* argv[])
{	
	char str[1024], c;
	char inputFile[64]="";
	char outputFile[64]="";
	int i = 0;

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
		for (i=0,c=getchar();c!=';'&&c!=EOF;c=getchar())
			str[i++]=c;
		str[i]=';';
		str[i+1] = '\0';
		if (c == EOF) {
			if (i) //结束非空
				printf("error\n");
			break;
		}
		if(processCmd(str)) //返回为非零报错
			printf("error\n");
	} while (1);
	
	return 0;
}