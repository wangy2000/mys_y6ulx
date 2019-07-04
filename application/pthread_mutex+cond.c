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

#define pthread1_size 0x5

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
int info1=0;
int info2;

void *pthread_fun1(void *value)
{
	printf("run : pthread_fun1.\n");
	pthread_mutex_lock(&mutex1);
	printf("lock : info1: %d .\n",info1);
	printf("B wait : info1: %d .\n",info1);
	pthread_cond_wait(&cond1,&mutex1);
	usleep(1000000);
	info1++;
	printf("A wait : info1: %d .\n",info1);
	usleep(1000000);
	pthread_mutex_unlock(&mutex1);
	printf("unlock .\n");
	usleep(1000000);
//	pthread_cond_signal(&cond1);
	printf("signal .\n");

}


int main(int argc, char * argv[])
{
	int error1;
	pthread_t pthread1[pthread1_size];
	
	for(int i=0;i<pthread1_size;i++)
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
	sleep(1);
	pthread_cond_broadcast(&cond1);
	printf("main signal\n");
	sleep(20);
	printf(" destroy status: %d.%s \n",pthread_mutex_destroy(&mutex1),\
		strerror(errno));
	printf(" destroy status: %d.%s \n",pthread_cond_destroy(&cond1),\
		strerror(errno));

		
}
