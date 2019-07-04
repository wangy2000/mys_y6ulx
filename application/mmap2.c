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

#define context1 "robe\n"
#define context "robe.zhang"
#define buf_size 0x10


struct stat stat1;
char buffer_string[0x10];

int main(int argc, char * argv[])
{
	int fd1;
	off_t offset1;
	pid_t pid1,pidc;


	int error1;

	void *mmap1;
	size_t  length=buf_size;

	fd1=open("/tmp/robe1", O_RDWR| O_CREAT| O_TRUNC,0777);
	if(fd1==-1){
		printf("error: open.\n");
		exit(0);
	}
	else{
		printf("ok:open.\n");
		
	}
	
	write(fd1,context1,sizeof(context1));

	if(error1=fstat(fd1,&stat1)==-1)
		{
		printf("error:stat.\n");
		close(fd1);
		exit(0);
		}
	else{
		printf("ok : stat. \n");
	}
	printf("file size: %lu \n",stat1.st_size);
	for( long int i=0;i<stat1.st_size;i++)
		{
		lseek(fd1,i,SEEK_SET);
		read(fd1,buffer_string,1);
		printf("%lu: char:< %c >. hex:< %x >.\n",i,\
			buffer_string[0],buffer_string[0]);
		}
	close(fd1);


	
/*	mmap1=mmap(NULL, buf_size,  PROT_EXEC|PROT_READ| PROT_WRITE,MAP_SHARED,\
                  fd1, 0);
	close(fd1);
	if(mmap1==(void*)-1){
		printf("error:mmap.\n");
		exit(0);
	}else{
		printf("OK:mmap.\n");
	}
	printf("parent: mmap1 = %p .\n",mmap1);
	printf("parent: before child, /tmp/robe1: %s .\n",(char*)mmap1);
	pid1=fork();
	
	if(pid1==0)
		{
			sleep(1);
			printf("child: mmap1 = %p .\n",mmap1);
			memset(mmap1,'C',buf_size-1);
			memset(mmap1+buf_size-1,'\0',1);
			printf("child: OK,sprintf: %s .\n",(char*)mmap1);
			printf("child.exit.\n");
			pidc=getpid();
			exit(0);
		}
	waitpid(pidc,NULL,0);
	sleep(1);
	printf("parent: mmap1 = %p .\n",mmap1);
	printf("parent: after child, sprintf: %s .\n",(char*)mmap1);
	memcpy(mmap1,context,sizeof(context));
	printf("context: %s . size : %lu.\n",context,sizeof(context));
	printf("parent: after memcpy, sprintf: %s .\n",(char*)mmap1);
	munmap(mmap1,buf_size);
*/	return(0);
}
