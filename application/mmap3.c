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

#define shm_size 0xff

#define shm_name "/tmp_shm1_robe"

#define sem_name "/tmp_sem1_robe"


struct shm{
	int shm1;
	sem_t sem1;
	void *mmap1;
};


int main(int argc, char * argv[])
{
	struct shm shma;
	int error1;
	int i=0;

	if(argc!=2)
		{printf("usage: main s/p\n");
		printf("main s :for sending string.\nmain p :for printing string.\n");
		exit(0);
	}

/**/
	shma.shm1=shm_open(shm_name,O_CREAT|O_RDWR,0777);
	if(shma.shm1==-1)
		{
		printf("error:shm_open.\n");
		exit(0);
	}
	else
		{
		printf("ok:shm_open.\n");
	}
	error1=ftruncate(shma.shm1, shm_size);
	if(error1==-1)
		{
		printf("error:ftruncate.\n");
		shm_unlink(shm_name);
		exit(0);
	}
	else
		{
		printf("ok:ftruncate.\n");
	}

/* sem */	
	shma.sem1=*sem_open(sem_name, O_CREAT|O_RDWR,\
                       0777, 0);
	if(&shma.sem1==SEM_FAILED)
		{
		printf("error:sem_open.\n");
		shm_unlink(shm_name);
		exit(0);
	}
	else
		{
		printf("ok:sem_open.\n");
	}

/*==debug*/	
	printf("debug:shma.sem1 %p.\n",&shma.sem1);

	
	error1=sem_init(&shma.sem1, 1, 0);
	if(error1==-1)
		{
		printf("error:sem_init.\n");
		shm_unlink(shm_name);
		exit(0);
	}
	else
		{
		printf("ok:sem_init.\n");
	}

/**/	
	shma.mmap1=mmap(NULL, shm_size,  PROT_EXEC|PROT_READ| PROT_WRITE,MAP_SHARED,\
                  shma.shm1, 0);
	if(shma.mmap1==MAP_FAILED){
		printf("error:mmap.\n");
		exit(0);
	}else{
		printf("OK:mmap.\n");
	}
	if(*argv[1]=='p')
	while(1){
//		sem_wait(&shma.sem1);
		printf("%d:mmap:%s\n",i,(char *)shma.mmap1);
		memset(shma.mmap1,0,shm_size);
		sleep(1);
		i++;
//		sem_post(&shma.sem1);
		sleep(1);
		
	}
	if(*argv[1]=='s')
		{
//			sem_post(&shma.sem1);
			while(1){
			
//				sem_wait(&shma.sem1);
				printf("please input char:");
				scanf("%s",(char *)shma.mmap1);
				sleep(1);
//				sem_post(&shma.sem1);
				
			}
		}

	munmap(shma.mmap1,shm_size);
	sem_destroy(&shma.sem1);
	sem_unlink(sem_name);
	shm_unlink(shm_name);
	return(0);
}
