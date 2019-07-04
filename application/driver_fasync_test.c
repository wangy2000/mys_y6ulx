
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

static void callback_SIGIO(int signum){
	printf("[%d] signal[%d].\n",index_a++,signum);
	return;
}

int main(int argc, char **argv	){
	int flags;
	int fd;
	fd=open("/dev/robe_device_create",O_RDWR,S_IRUSR|S_IWUSR);
	if(fd==-1){
		printf("open file error..\n");
		return -1;
	}
	fcntl(fd,F_SETOWN,getpid());
	flags=fcntl(fd,F_GETFL);
	fcntl(fd,F_SETFL,flags|FASYNC);
	signal(SIGIO,callback_SIGIO);
	while(1);
	return 0;
}

//fcntl 修改 file descriptor 的函数，内核函数


