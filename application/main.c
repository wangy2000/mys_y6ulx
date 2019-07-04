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
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#if 0
struct sockaddr {
               sa_family_t sa_family;
               char        sa_data[14];
           }

struct sockaddr_in
  {
    __SOCKADDR_COMMON (sin_);
    in_port_t sin_port;			/* Port number.  */
    struct in_addr sin_addr;		/* Internet address.  */

    /* Pad to size of `struct sockaddr'.  */
    unsigned char sin_zero[sizeof (struct sockaddr) -
			   __SOCKADDR_COMMON_SIZE -
			   sizeof (in_port_t) -
			   sizeof (struct in_addr)];
  };
#endif

int main(int argc, char * argv[])
{
	int socketfd1,socketfd2;
	int error1;

	int numbytes;
	char buff[256];
	if(argc!=2){
		printf("usage : ./main s/c.\n");
		goto a;
	}

//	struct sockaddr socketaddr1;
	struct sockaddr_in sockaddr_in1,sockaddr_in2,sockaddr_in3;
	
	socketfd1=socket(AF_INET,SOCK_STREAM,0);
	if(socketfd1==-1){
		printf("error:socket.\n");
		goto a;
	}else{
		printf("OK:socket.\n");
	}
//=========================================================server
	if(*argv[1]=='s'){
		bzero(&sockaddr_in1, sizeof(struct sockaddr_in));
		sockaddr_in1.sin_family=AF_INET;
		sockaddr_in1.sin_port=htons(12345);		//uint16_t
//		sockaddr_in1.sin_addr.s_addr = inet_addr("192.168.8.200");
		error1=bind(socketfd1,(struct sockaddr*)&sockaddr_in1,\
			sizeof(struct sockaddr));
		if(error1==-1){
			printf("error:bind.\n");
			goto b;
		}else{
			printf("OK:bind.\n");
		}

		error1=listen(socketfd1,10);
		if(error1==-1){
			printf("error:listen.\n");
			goto b;
		}else{
			printf("OK:listen.\n");
		}

		int sockaddr_in2_size=sizeof(struct sockaddr_in);
		bzero(&sockaddr_in2, sizeof(struct sockaddr_in));
		sockaddr_in2.sin_family=AF_INET;
		sockaddr_in2.sin_port=htons(12345);		//uint16_t
//		sockaddr_in2.sin_addr.s_addr = inet_addr("192.168.8.2");
		while(1){
			socketfd2=accept(socketfd1,(struct sockaddr*)&sockaddr_in2,\
				&sockaddr_in2_size);
			if(socketfd2<0){
				printf("error:accept.\n");
				goto b;
			}else{
				printf("OK:accept.\n");
			}
			
			if(!fork())
			{
				int i=0;
				printf("OK:child process generate.<%d>.\n",getpid());
				while(1){
					memset(buff,0,sizeof(buff));
					numbytes=recv(socketfd2,buff,sizeof(buff),0);
					if(numbytes==-1){
						printf("error:child recv.\n");
						goto c;
					}
					if(numbytes==0){
						printf("return 0:child recv.\n");
						goto c;
					}
					printf("recv:%s.",buff);
					sprintf(buff+strlen(buff),"<%d>.",++i);
					send(socketfd2, buff, strlen(buff), 0);
					printf("send:%s.\n",buff);
				}
c:				printf("OK:child process exit.    <%d>.\n",getpid());
				close(socketfd2);
				exit(0);
			}
			
		}	
	}
//=========================================================client
	if(*argv[1]=='c'){
		bzero(&sockaddr_in3, sizeof(struct sockaddr_in));
		sockaddr_in3.sin_family=AF_INET;
		sockaddr_in3.sin_port=htons(12345);		//uint16_t
//		sockaddr_in3.sin_addr.s_addr = inet_addr("192.168.8.200");
		error1=connect(socketfd1,(struct sockaddr*)&sockaddr_in3,\
			sizeof(struct sockaddr));
		if(error1==-1){
			printf("error:connect.\n");
			goto b;
		}else{
			printf("OK:connect.\n");
		}
		while(1){
			memset(buff,0,sizeof(buff));
			printf("input:");
			scanf("%s",buff);
			send(socketfd1, buff, strlen(buff), 0);
			printf("send:%s.",buff);
			memset(buff,0,sizeof(buff));
			recv(socketfd1,buff,sizeof(buff),0);
			printf("recv:%s.\n",buff);
		}
		
	}
//=========================================================fail code
b:	close(socketfd1);
a:	exit(0);
}
