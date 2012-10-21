#include <stdio.h>
#include <string.h>
#include "DatabaseAPI.h"
#include "CmdProcess.h"

/*
 *Parameters:
 *	@proName:The name of the execution file.
 *Return:
 *	None
 *Description:
 *	Print the usage of the program.
 *Date:
 *	2012-10-21 10:01:01
 */
void usage(char *proName)
{
	printf("Usage:\n");
	printf(" %s <input file name> <output file name>\n\n", proName);
	printf("Options:\n");
	printf(" <input file name>  file which contains test cases\n");
	printf(" <output file name> file which show the outcome\n\n");
}

int main(int argc, char* argv[])
{	
	char str[1024], c;
	int i;

	/* check argument count */
	if (argc < 3)
	{
		usage(argv[0]);
		return 0;
	}

	/* redirect stand input/output stream */
	if(freopen(argv[1], "r", stdin)==NULL)
	{
		printf("can not open %s\n", argv[1]);
		return -1;
	}
	freopen(argv[2], "w", stdout);

	/* process commands */
	do 
	{
		for (i = 0, c = getchar(); c != ';'; c = getchar())
			str[i++] = c;
		str[i] = '\0';

		processCmd(str);
	}while(c != EOF);
		
	return 0;
}