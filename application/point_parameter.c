
/*	**********************************************

使用函数中的参数，对外返回结果。

运行结果：
 [change_value]  1, 0.
 [change_value]  2, 1.
 [change_value]  3, 2.
 [change_value]  4, 3.
 [change_value]  5, 4.
 [change_value]  6, 5.
 [change_value]  7, 6.
 [change_value]  8, 7.
 [change_value]  9, 8.
 [change_value]  10, 9.


 *	**********************************************
 */


#include <stdio.h>
#include <linux/input.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

int change_value(int *value){
	printf("[change_value]  %d, %d.\n", *value,(*value)++);
	return 0;
}


int main(void)
{
	int data = 0 ;
	int i = 0;
	for(i=0;i<10;i++)
		change_value(&data);
	return 0; 

}























