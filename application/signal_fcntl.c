
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#define MAX_LEN 0x20

int index_a=0;

static void callback_SIGIO(int a){
	char data[MAX_LEN];
	int len;

	len=read(STDIN_FILENO,&data,MAX_LEN);
	data[len] = 0;
	printf("[%d] input string : %s .",index_a++,data);
	return;
}

int main(int argc, char **argv	){
	int flags;

	fcntl(STDIN_FILENO,F_SETOWN,getpid());
	flags=fcntl(STDIN_FILENO,F_GETFL);
	fcntl(STDIN_FILENO,F_SETFL,flags|FASYNC);
	signal(SIGIO,callback_SIGIO);
	while(1);
	return 0;
}

//  robe@7060 # kill -L  列出所有的信号


