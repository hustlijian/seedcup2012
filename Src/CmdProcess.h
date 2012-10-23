#ifndef CMDPROCESS_H
#define CMDPROCESS_H

#define NUM_KEYWORDS 33
#define MAX_KEYWORDS_LENGTH 10
#define NUM_OPERATOR	20
#define MAX_OPERATOR	3
#define NUM_SPACE		3
#define MAX_WORDLEGTH  1024

#define SYN_CREATE		1
#define SYN_DATABASE	2
#define SYN_TABLE		3
#define SYN_ALTER		4
#define SYN_TRUNCATE	5
#define SYN_ADD			6
#define SYN_COLUMN		7
#define SYN_USE			8
#define SYN_DROP		9
#define SYN_RENAME		10
#define SYN_SELECT		11
#define SYN_FROM		12
#define SYN_WHERE		13
#define SYN_ORDER		14
#define SYN_BY			15
#define SYN_DESC		16
#define SYN_INCR		17
#define SYN_UPDATE		18
#define SYN_SET			19
#define SYN_DELETE		20
#define SYN_INSERT		21
#define SYN_INTO		22
#define SYN_VALUES		23
#define SYN_SHOW		24
#define SYN_DATABASES	25
#define SYN_INT			26
#define SYN_FLOAT		27
#define SYN_TEXT		28
#define SYN_NONE		29
#define SYN_BETWEEN		30
#define SYN_LIKE		31
#define SYN_AND			32
#define SYN_OR			33

#define SYN_IDENTIFIER	40
#define SYN_INTEGER_NUMBER	41
#define SYN_FLOAT_NUMBER	42

#define SYN_OPERATOR_BASE	50
#define SYN_PAREN_LEFT	50
#define SYN_PAREN_RIGHT	51
#define SYN_MULT		52
#define SYN_DIVI		53
#define SYN_MOD			54
#define SYN_PULSE		55
#define SYN_MIMUS		56
#define SYN_EQUAL		57
#define SYN_NOT_EQUAL	58
#define SYN_MORE_EQUAL	59
#define SYN_LESS_EQUAL	60
#define SYN_GREATER		61
#define SYN_LESS		62
#define SYN_BRACKET_LEFT	63
#define SYN_BRACKET_RIGHT	64
#define SYN_SEMICOLON	65
#define SYN_COMMA		66
#define SYN_QUOTE		67
#define SYN_QUESTION	68
#define SYN_ASSIGN		69
#define SYN_ELSE		0


/*在返回值为int的函数中，返回-1代表失败，返回0代表成功*/
int processCmd(char *cmd);
void test();
#endif //CMDPROCESS_H