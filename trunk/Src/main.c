#include <stdio.h>
#include <string.h>
#include "DatabaseAPI.h"
#include "CmdProcess.h"

int main(int argc, char* argv[])
{	
	char str[1024], c;
	int i;
	/*
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
	*/
#if 0
	test();
#else
	freopen("interface.txt", "r", stdin); 
	freopen("interface_out.txt","w", stdout);
	do 
	{
		for (i=0,c=getchar();c!=';'&&c!=EOF;c=getchar())
			str[i++]=c;
		str[i]='\0';

		if(processCmd(str)) //返回为非零报错
			printf("error\n");
	} while (c!=EOF);
#endif		
	return 0;
}