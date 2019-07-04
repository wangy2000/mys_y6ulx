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

#define FIFO1 "/tmp/robe1"
#define FIFO2 "/tmp/robe2"

#define BUF_SIZE 0xff

#define AUTH "|"

int main(int argc,char *argv[] )
{
	int procid;
	char buf[BUF_SIZE];
	char buf1[BUF_SIZE];
	char info_buf[BUF_SIZE];
	int readfd;
	int writefd;
	int len;
	int info=0;
	
	if(argc != 2)
	{
		printf("usage: main client\n\t main server \n");
		exit(0);
	}

	if( (*argv[1]) == 's')
	{
		readfd=open(FIFO1,O_RDONLY);
		writefd=open(FIFO2,O_WRONLY);
		if(readfd==-1)
		{
			printf("server: fifo1 open file. \n");
			exit(0);
		}
		else
		{
			printf("server: fifo1 open sucessed. \n");
		}
		if(writefd==-1)
		{
			printf("server: fifo2 open file. \n");
			exit(0);
		}
		else
		{
			printf("server: fifo2 open sucessed. \n");
		}
		while(1)
		{	
			memset(buf,0,BUF_SIZE);
			len=read(readfd,buf,BUF_SIZE);
			printf("server receive %d byte. string is %s\n",len,buf);
			info++;
			sprintf(info_buf,"%d",info);
			strcat(buf,AUTH);
			strcat(buf,info_buf);
			len = strlen(buf);
			sleep(1);
			write(writefd,buf,len);	
		}
	}
	if( (*argv[1]) == 'c' )
	{
		writefd=open(FIFO1,O_WRONLY);
		readfd=open(FIFO2,O_RDONLY);
		if(writefd==-1)
		{
			printf("client: fifo1 open file. \n");
			exit(0);
		}
		else
		{
			printf("client: fifo1 open sucessed. \n");
		}
		if(readfd==-1)
		{
			printf("client: fifo2 open file. \n");
			exit(0);
		}
		else
		{
			printf("client: fifo2 open sucessed. \n");
		}
		memset(buf1,0,BUF_SIZE);
		fscanf(stdin,"%s",buf1);
		len=strlen(buf1);
		printf("len : %d, string = %s \n",len,buf1);
		while(1)
		{
			
						
			len=write(writefd,buf1,len);			
			printf("client write:%d byte. string is : %s \n",len,buf1);
			memset(buf1,0,BUF_SIZE);
			len=read(readfd,buf1,BUF_SIZE);
			printf("client read:%d byte. string is : %s \n",len,buf1);
		}
	}
}
	
/* 
	procid=fork();
	
	if(procid==0)
	{
		readfd=open(FIFO1,O_RDONLY);
		if(readfd==-1)
		{
			printf("child: fifo1 open file. \n");
			exit(0);
		}
		else
		printf("child: fifo1 open ok. \n");
		while
		{
			memset(buf1,0,BUF_SIZE);
			while(len=read(readfd,buf1,BUF_SIZE)<=0);
			write(STDOUT_FILENO,buf1,len);
			
		}
	}
	else
	{
		writefd=open(FIFO1,O_WRONLY);
		if(writefd==-1)
		{
			printf("parent: fifo1 open file. \n");
			exit(0);
		}
		else
		printf("parent: fifo1 open ok. \n");
		while(1)
		{
			memset(buf,0,BUF_SIZE);
			while(fscanf(stdin,"%s",buf)==0);
			printf("parent: input: %s, \n",buf);
			strcat(buf,AUTH);
			len=strlen(buf);
			write(writefd,buf,len);
			printf("parent: send: %s, \n",buf);
		}
	}

 */

