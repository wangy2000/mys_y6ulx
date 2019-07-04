
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

int fun(int a){
	printf("fun.\n");
}

int main(int argc, char **argv	){
	int (* funp)(int ),(*funp2)(int );
	funp = fun ;
	funp2 = &fun;
	printf("funp  [%p].\n",funp);
	printf("funp2 [%p].\n",funp2);
	printf("fun   [%p].\n",fun);
	printf("fun&  [%p].\n",&fun);
	return 0;
}



