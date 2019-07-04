#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define NAME "robe.zhang"

union semun {
   int              val;    /* Value for SETVAL */
   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
   unsigned short  *array;  /* Array for GETALL, SETALL */
   struct seminfo  *__buf;  /* Buffer for IPC_INFO
							   (Linux-specific) */
}; 

int sem_creat(int semkey)
{
	int sem_id;
	sem_id = semget(semkey, 1, 0777 | IPC_CREAT);
	return sem_id;
}

int set_value(int semid,int val)
{
    union semun sem_union;

    sem_union.val = val;
    if(semctl(semid, 0, SETVAL, sem_union) == -1) return 0;
    return 1;
}

int sem_operation(int semid,int val)
{
    struct sembuf sem_b;

    sem_b.sem_num = 0;
    sem_b.sem_op = val;
    sem_b.sem_flg = SEM_UNDO;
    if(semop(semid, &sem_b, 1) == -1)
    {
        printf("semoperation error.\n");
        return -1;
    }
    return 0;
}

void del_sem(int semid)
{
    union semun sem_union;

    if(semctl(semid, 0, IPC_RMID, sem_union) == -1)
        printf("delete sem error.\n");
}

int read_info(void)
{
	FILE *file1;
	int buf;	
	file1=fopen("/datadisk/github/test/info","r+");
	fscanf(file1,"%d",&buf);
	fclose(file1);
	return buf;
}

void write_info(int value)
{	
	FILE *file1;
	file1=fopen("/datadisk/github/test/info","r+");
	fprintf(file1,"%d",value);
	fclose(file1);
}

void fun_a(int a)
{
	printf("fun_a: pid: %d \n",getpid());
	exit(100);
}

void fun_b(int a)
{
	printf("fun_b: pid: %d \n",getpid());
	exit(100);
}

int info=0;

int main(void)
{
	int procid;
	
	int sem_id;
	
	sem_id=sem_creat(getpid());
	
	set_value(sem_id,0);
	sem_operation(sem_id,1);
	
	for(int i=0;i<2;i++)
		fork();
	
	signal(SIGINT,fun_a);
	signal(SIGALRM,fun_b);
	
	
	while(1)
	{
		while(sem_operation(sem_id,-1)!=0);
		info = read_info();
		printf("pid:%d  info = %d. \n",getpid(),info);
		info=info+1;
		write_info(info);
		usleep(1000000);
		sem_operation(sem_id,1);
	}
}
