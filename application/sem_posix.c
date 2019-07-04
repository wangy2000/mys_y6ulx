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

#define semname1 "/robe.zhang"

sem_t *sem1;

void fun_thread1(void *value)
{
	printf("fun_thread1: .\n");
	int error2;
	while(1)
		{
		sem_wait(sem1);
		printf("sem_wait .\n");
		}
}


int main(int argc, char * argv[])
{
	int error1;
	pthread_t thread1;
		
	sem1=sem_open(semname1,O_CREAT,0777,0);
	if(sem1==SEM_FAILED)
		{
		printf("error: sem_open.\n");
		exit(0);
		}
	else
		{
		printf("OK: sem_open.\n");
		}
	error1=sem_init(sem1,0,0);
	if(error1!=0)
		{
		printf("error: sem_init.\n");
		sem_close(sem1);
		exit(0);
		}
	else
		{
		printf("OK : sem_init.\n");
		}
	pthread_create(&thread1,NULL,fun_thread1,NULL);
		
	while(1)
		{
		sleep(1);
		sem_post(sem1);
		printf("sem_post .\n");
		
		}
	
}
