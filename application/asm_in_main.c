#include<stdio.h>

void main(void)
{
	int a=1;
	int b=2;
	int c=3;
	int d=4;
	int e;
	__asm__ __volatile__(
		"mov r0,%1\n"
		"add r0,r0,%2\n"
		"add r0,r0,%3\n"
		"add r0,r0,%4\n"
		"mov %0,r0"
		:"=r"(e)
		:"r"(a),"r"(b),"r"(c),"r"(d)
		:"r0"
	);
	printf("%d + %d + %d + %d = %d \n", a,b,c,d,e);
	return ;
}