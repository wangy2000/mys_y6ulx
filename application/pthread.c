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

#define pthread1_size 0x1

unsigned int i;


pid_t pid1;

void *pthread_fun1(void *arg1)
{
	printf("run: pthread_fun1. pid=%d. \n",getpid());
	sleep(1);
	pthread_exit(NULL);
}

int main(int argc, char * argv[])
{
	int error1;
	pthread_t pthread1[pthread1_size];

	pid1= getpid();
	
	for(i=0;i<pthread1_size;i++)
	{
		error1=pthread_create(&pthread1[i],NULL,pthread_fun1,NULL);
		if(error1==-1)
		{
			printf("error: pthread_create.\n");
			exit(1);
		}
		else
		{
			printf("ok : pthread_create. pthread id: %d < %u >\n",\
				i,(unsigned int)pthread1[i]);	
		}
		
	}
	error1=pthread_join(pthread1[0],NULL);
	if(error1==0)
		{
		printf("ok: pthread_join.\n");
		sleep(1);
		exit(0);
		}
		

}
