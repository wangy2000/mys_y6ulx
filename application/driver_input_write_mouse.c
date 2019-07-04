#include <stdio.h>
#include <linux/input.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
/*
struct input_event {
        struct timeval time;
        __u16 type;
        __u16 code;
        __s32 value;
};

Event types
#define EV_SYN                  0x00
#define EV_KEY                  0x01
#define EV_REL                  0x02
#define EV_ABS                  0x03

 Relative axes
#define REL_X                   0x00
#define REL_Y                   0x01
#define REL_Z                   0x02
#define REL_RX                  0x03
#define REL_RY                  0x04
#define REL_RZ                  0x05
#define REL_HWHEEL              0x06
#define REL_DIAL                0x07
#define REL_WHEEL               0x08
#define REL_MISC                0x09
#define REL_MAX                 0x0f
#define REL_CNT                 (REL_MAX+1)
*/

#define MOUSE_EVENT "/dev/input/mouse0"    // event13,event15,event16

int main(void)
{
	struct input_event event_mouse ;
	int fd = open(MOUSE_EVENT,O_RDWR);
	int value ;
	int type ;
	int i ;
	int buffer[10]={0};
	if(-1 == fd){
		printf("open mouse event fair!\n");
		return -1 ;
	}	
	while(1){
		for(i = 0 ; i < 20 ; i++){
		   event_mouse.type = EV_REL ; 
		   event_mouse.code = REL_X ;
		   event_mouse.value = 10*i ; 	
		   write(fd,&event_mouse,sizeof(event_mouse));		   
		   event_mouse.code = 0 ;
		   event_mouse.value = 0;
		   event_mouse.type = EV_SYN ;
		   write(fd,&event_mouse,sizeof(event_mouse));		   
		   usleep(50000);
		}
		for(i = 0 ; i < 20 ; i++){
		   event_mouse.type = EV_REL ; 
		   event_mouse.code = REL_Y ;
		   event_mouse.value = 10*i ; 	
		   write(fd,&event_mouse,sizeof(event_mouse));		   
		   event_mouse.code = 0 ; 
		   event_mouse.value = 0 ;
		   event_mouse.type = EV_SYN ;
		   write(fd,&event_mouse,sizeof(event_mouse));		   
		   usleep(50000);
		}
		for(i = 0 ; i > -20 ; i--){
		   event_mouse.type = EV_REL ; 
		   event_mouse.code = REL_X ;
		   event_mouse.value = 10*i ; 	
		   write(fd,&event_mouse,sizeof(event_mouse));		   
		   event_mouse.code = 0 ;
		   event_mouse.value = 0;
		   event_mouse.type = EV_SYN ;
		   write(fd,&event_mouse,sizeof(event_mouse));		   
		   usleep(50000);
		}
		for(i = 0 ; i > -20 ; i--){
		   event_mouse.type = EV_REL ; 
		   event_mouse.code = REL_Y ;
		   event_mouse.value = 10*i ; 	
		   write(fd,&event_mouse,sizeof(event_mouse));		   
		   event_mouse.code = 0 ; 
		   event_mouse.value = 0 ;
		   event_mouse.type = EV_SYN ;
		   write(fd,&event_mouse,sizeof(event_mouse));		   
		   usleep(50000);
		}
		printf("write mouse event ok!\n");
	}	
	return 0 ;
}


