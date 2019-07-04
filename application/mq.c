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
#include <sys/sem.h>
#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>

#define buf_size 0x2000

#define msgk1 "/robe1"
#define msgk2 "/robe2"


mqd_t mqid1,mqid2;
struct mq_attr attr1,attr2;

void sys_info(mqd_t value)
{
	if(value==-1)
	{
		printf("mq_open error.\n");
		printf("error:%s\n",strerror(errno));
		exit(1);
	}
	else
	{
		printf("mq_open sucessed!\n");
	}
}

void fun_alarm(int value)
{
	mq_close(mqid1);
	mq_close(mqid2);
 	mq_unlink(msgk1);
	mq_unlink(msgk2);
	printf("fun_alarm.\n");
	exit(0);
}

void printf_attr(struct mq_attr *attr)
{
	printf("mq_flags:  %lu .\n",attr->mq_flags);
	printf("mq_maxmsg: %lu .\n",attr->mq_maxmsg);
	printf("mq_msgsize:%lu .\n",attr->mq_msgsize);
	printf("mq_curmsgs:%lu .\n",attr->mq_curmsgs);
}

int main(int argc,char *argv[] )
{
	
	char buf[buf_size];
	int index=0;
	int err;
	unsigned int proi1=1;
	unsigned int proi2=2;
	unsigned int proi3=3;	
	unsigned int proi4=4;
	unsigned int proi5=5;
	ssize_t num;
	
	mqid1=mq_open(msgk1,O_RDWR|O_CREAT,0777,NULL);
	sys_info(mqid1);

	mq_getattr(mqid1,&attr1);	
	printf_attr(&attr1);	
	
	if(argc!=2)
	{
		printf("usage : ./main s \n        ./main a/b/c \n");
				  exit(0);
	}
	
	while(*argv[1]=='s')
	{
		memset(buf,0,buf_size);
		sprintf(buf,"%d",index++);
		mq_send(mqid1,buf,strlen(buf),proi1);
		printf("mq_send mqid1: %s \n",buf);	
		memset(buf,0,buf_size);
		sprintf(buf,"%d",index++);
		mq_send(mqid1,buf,strlen(buf),proi2);
		printf("mq_send mqid1: %s \n",buf);
		memset(buf,0,buf_size);
		sprintf(buf,"%d",index++);
		mq_send(mqid1,buf,strlen(buf),proi3);
		printf("mq_send mqid1: %s \n",buf);
		memset(buf,0,buf_size);
		sprintf(buf,"%d",index++);
		mq_send(mqid1,buf,strlen(buf),proi4);
		printf("mq_send mqid1: %s \n",buf);
		memset(buf,0,buf_size);
		sprintf(buf,"%d",index++);
		mq_send(mqid1,buf,strlen(buf),proi5);
		printf("mq_send mqid1: %s \n",buf);
		usleep(1000000);	
	}
	while(*argv[1]=='a')
	{
		memset(buf,0,buf_size);
		num = mq_receive(mqid1,buf,buf_size,&proi1);
		if(num>0)
			printf("mq_receive mqid1: %s \n",buf);
	}
	while(*argv[1]=='b')
	{
		memset(buf,0,buf_size);
		num = mq_receive(mqid1,buf,buf_size,&proi2);
		if(num>0)
			printf("mq_receive mqid1: %s \n",buf);
	}
	while(*argv[1]=='c')
	{
		memset(buf,0,buf_size);
		num = mq_receive(mqid1,buf,buf_size,&proi3);
		if(num>0)
			printf("mq_receive mqid1: %s \n",buf);
	}
	while(*argv[1]=='d')
	{
		memset(buf,0,buf_size);
		num = mq_receive(mqid1,buf,buf_size,&proi4);
		if(num>0)
			printf("mq_receive mqid1: %s \n",buf);
	}
	while(*argv[1]=='e')
	{
		memset(buf,0,buf_size);
		num = mq_receive(mqid1,buf,buf_size,&proi5);
		if(num>0)
			printf("mq_receive mqid1: %s \n",buf);
	}
	
}

