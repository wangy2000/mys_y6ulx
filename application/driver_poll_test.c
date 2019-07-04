
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>


int main(int argc, char **argv	){
	int fd,num;
	fd_set rfds,wfds;
	int index;

	fd=open("/dev/robe_device_create",O_RDONLY|O_NONBLOCK);
	
	if(fd==-1){
		printf("device open failure.\n");
		return -1;
	}
	index=0;
	while(1){
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		FD_SET(fd,&rfds);
		FD_SET(fd,&wfds);
		select(fd+1,&rfds,&wfds,NULL,NULL);
		printf("[%d]pool monitor : device can be ",index++);
		if(FD_ISSET(fd,&rfds))
			printf("read");
		printf(" ");
		if(FD_ISSET(fd,&wfds))
			printf("write");
		printf(" .\n");
		sleep(1);
	}
	return 0;
}




