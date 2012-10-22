#ifndef CMDPROCESS_H
#define CMDPROCESS_H

#define NUM_KEYWORDS 33
#define MAX_KEYWORDS_LENGTH 10
#define NUM_OPERATOR	20
#define MAX_OPERATOR	3
#define NUM_SPACE		3
#define MAX_WORDLEGTH  1024

/*在返回值为int的函数中，返回-1代表失败，返回0代表成功*/
int processCmd(char *cmd);
void test();
#endif //CMDPROCESS_H