#ifndef CMDPROCESS_H
#define CMDPROCESS_H

#define NUM_KEYWORDS 33
#define MAX_KEYWORDS_LENGTH 10
#define NUM_OPERATOR	20
#define MAX_OPERATOR	3
#define NUM_SPACE		3
#define MAX_WORDLEGTH  1024

/*�ڷ���ֵΪint�ĺ����У�����-1����ʧ�ܣ�����0����ɹ�*/
int processCmd(char *cmd);
void test();
#endif //CMDPROCESS_H