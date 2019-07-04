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

#define msgbuf1_len 0xff

typedef struct msgbuf1 {
               long mtype;       /* message type, must be > 0 */
               char mtext[msgbuf1_len];    /* message data */
}message1;

message1 msg1;

void fun_alrm(int value)
{
	printf("fun_alarm.\n");
	exit(0);
}


int main(int argc, char * argv[])
{
	key_t key1;
	int msgid1;
	int err;
	ssize_t err1;

	unsigned int num;
	
	if(argc!=6)
	{
	printf("usage : ./main s/c cycles mtype num add\n");
	exit(1);
	}
	
	key1=ftok("/robe2",0x80);
		
	if(key1==-1)
	{
		printf("ftok error. \n");
		exit(0);
	}

	msgid1=msgget(key1,IPC_CREAT|0777);
	if(msgid1==-1)
	{
		printf("msgget error .\n");
		exit(1);
	}else
	{
		printf("msgget ok .\n");
	}

	if(*argv[1]=='s')
	{

		signal(SIGALRM,fun_alrm);
		alarm(atoi(argv[2]));
		num = atoi(argv[4]);
		
		while(1)
		{
			msg1.mtype = atoi(argv[3]);
			sprintf(msg1.mtext,"%d",num);
			err=msgsnd(msgid1,&msg1,msgbuf1_len,0);
			if(err==-1)
			{
				printf("msgsnd error .\n");
				exit(2);
			}
			else
			{
				printf("msgsnd ok. msg:%s \n",msg1.mtext);
			}
			num+=atoi(argv[5]);
			sleep(1);
		}
		
	}
	if(*argv[1]=='r')
	{
		while(1)
		{
			memset(&msg1,0,sizeof(msg1));
			err1=msgrcv(msgid1,&msg1,msgbuf1_len,101,0);
			if(err1==-1)
			{
				printf("msgrcv error .\n");
				exit(2);
			}
			else
			{
				printf("msgrcv ok. \n");
				printf("msg:%s \n",msg1.mtext);
			}
		}
		
	}
	
}
