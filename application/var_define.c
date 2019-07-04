
/*	**********************************************

 验证定义变量的时候，*对变量的作用范围，*只对当前变量有效
 
 =============== char a, *b,c; ==========(x86-64 电脑运行结果)
  sizeof(a): 1.
  sizeof(b): 8.
  sizeof(c): 1.

  
 *	**********************************************
 */


#include <stdio.h>
#include <linux/input.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

int main(void)
{

	char a, *b,c;
	printf("=============== char a, *b,c; ==========\n");

	printf(" sizeof(a): %lu.\n",sizeof(a));
	printf(" sizeof(b): %lu.\n",sizeof(b));
	printf(" sizeof(c): %lu.\n",sizeof(c));

	return 0; 

}























