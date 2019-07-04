#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <pthread.h>
#include<semaphore.h>
#include<sys/mman.h>

#define context1 "mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm"
#define context "robe.zhang"
#define buf_size 0x10

//映射的长度要小于文件的长度，映射的空间全部在文件大小范围内
int main(int argc, char * argv[])
{
	int fd1;
	off_t offset1;
	pid_t pid1,pidc;


	int error1;

	void *mmap1;
	size_t  length=buf_size;

	fd1=open("/tmp/robe1", O_RDWR| O_CREAT,0777);
	if(fd1==-1){
		printf("error: open.\n");
		exit(0);
	}
	else{
		printf("ok:open.\n");
		
	}
	write(fd1,context1,sizeof(context1));
	mmap1=mmap(NULL, buf_size,  PROT_EXEC|PROT_READ| PROT_WRITE,MAP_SHARED,\
                  fd1, 0);
	close(fd1);
	if(mmap1==(void*)-1){
		printf("error:mmap.\n");
		exit(0);
	}else{
		printf("OK:mmap.\n");
	}
	pid1=fork();
	
	if(pid1==0)
		{
			sleep(1);
			printf("child: mmap1 = %p .\n",mmap1);
			memset(mmap1,'C',buf_size);
			printf("child: OK,sprintf: %s .\n",context);
			printf("child.exit.\n");
			pidc=getpid();
			exit(0);
		}
	waitpid(pidc,NULL,0);
	sleep(1);
	printf("parent: OK, sprintf: %s .\n",(char*)mmap1);
	memcpy(mmap1,context,sizeof(context));
	printf("parent: OK, sprintf: %s .\n",(char*)mmap1);
	munmap(mmap1,buf_size);
	return(0);
}
