/*====================================== h 头文件，全局变量，结构体，枚举类型  */
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

#define CHAR_SIZE 0x80

#define MENU_SIZE array_size()

#define MODULE_ITEM(item_x,menu,type_x,fn_x) \
	item_type item_x={ \
		.name=menu, \
		.type=type_x, \
		.fn_cmd=fn_x, \
	};


char input[CHAR_SIZE];
int error_info;
volatile int menu_color=37;
volatile int menu_bg_color=40;

typedef struct item{
	char name[CHAR_SIZE];
	int type;
	int (*fn_cmd)(void);
}item_type;

enum type{
	e_menu,
	e_item,
	e_max,
};
/* ==========================================================  菜单项 
 * 增加，删除菜单项方法：
 * 1，实现功能函数，并使用MODULE_ITEM声明一个菜单项
 *	int fn_a(void)
 *	{
 *		printf("sleeping......\n");
 *		sleep(1);
 *		printf("sucessed!\n");
 *		return 0;
 *	}
 *	MODULE_ITEM(item_a,"a:	delay 1 sec.",e_item,fn_a)
 * 2，把菜单项(item_a)添加到第 240 行的 menu 列表中。
 *	 item_type *menu[]={
 *		&item_a,
 *		&item_b,
 *	 }
 */
int fn_a(void)
{
	printf("sleeping......\n");
	sleep(1);
	printf("sucessed!\n");
	return 0;
}
MODULE_ITEM(item_a,"a:	delay 1 sec.",e_item,fn_a)

int fn_b(void)
{
	printf("hello,world.\n");
	return 0;
}
MODULE_ITEM(item_b,"b:	print \"hello,world!\"",e_item,fn_b)

int fn_c(void)
{
	printf("beep......\n");
	system("echo -n \"\a\" ");
	return 0;
}
MODULE_ITEM(item_c,"c:	beep",e_item,fn_c)

int fn_d(void)
{
	system("echo \"\e[32m	hello linux!\e[0m\"");
	return 0;
}
MODULE_ITEM(item_d,"d:	print green font",e_item,fn_d)

int fn_e(void)
{
	system("echo \"\e[33m	hello linux!\e[0m\"");
	return 0;
}
MODULE_ITEM(item_e,"e:	print yellow font",e_item,fn_e)

int fn_f(void)
{
	system("echo \"\e[34m	hello linux!\e[0m\"");
	return 0;
}
MODULE_ITEM(item_f,"f:	print blue font",e_item,fn_f)

int fn_g(void)
{
	system("echo \"\e[35m	hello linux!\e[0m\"");
	return 0;
}
MODULE_ITEM(item_g,"g:	print 紫色 font",e_item,fn_g)

int fn_h(void)
{
	system("echo \"\e[36m	hello linux!\e[0m\"");
	return 0;
}
MODULE_ITEM(item_h,"h:	print 青色 font",e_item,fn_h)

int fn_i(void)
{
	menu_color=31;
	return 0;
}
MODULE_ITEM(item_i,"i:	change menu font color to red",e_item,fn_i)

int fn_j(void)
{
	menu_color=32;
	return 0;
}
MODULE_ITEM(item_j,"j:	change menu font color to green",e_item,fn_j)

int fn_k(void)
{
	menu_color=33;
	return 0;
}
MODULE_ITEM(item_k,"k:	change menu font color to yellow",e_item,fn_k)

int fn_l(void)
{
	menu_color=34;
	return 0;
}
MODULE_ITEM(item_l,"l:	change menu font color to blue",e_item,fn_l)

int fn_m(void)
{
	menu_color=35;
	return 0;
}
MODULE_ITEM(item_m,"m:	change menu font color to 紫色",e_item,fn_m)

int fn_n(void)
{
	menu_color=36;
	return 0;
}
MODULE_ITEM(item_n,"n:	change menu font color to 青色",e_item,fn_n)

int fn_o(void)
{
	menu_color=30;
	return 0;
}
MODULE_ITEM(item_o,"o:	change menu font color to black",e_item,fn_o)

int fn_p(void)
{
	menu_color=37;
	return 0;
}
MODULE_ITEM(item_p,"p:	change menu font color to default",e_item,fn_p)

int fn_q(void)
{
	menu_bg_color=41;
	return 0;
}
MODULE_ITEM(item_q,"q:	change menu background color to red",e_item,fn_q)

int fn_r(void)
{
	menu_bg_color=42;
	return 0;
}
MODULE_ITEM(item_r,"r:	change menu background color to green",e_item,fn_r)

int fn_s(void)
{
	menu_bg_color=43;
	return 0;
}
MODULE_ITEM(item_s,"s:	change menu background color to yellow",e_item,fn_s)

int fn_t(void)
{
	menu_bg_color=44;
	return 0;
}
MODULE_ITEM(item_t,"t:	change menu background color to blue",e_item,fn_t)

int fn_u(void)
{
	menu_bg_color=45;
	return 0;
}
MODULE_ITEM(item_u,"u:	change menu background color to 紫色",e_item,fn_u)

int fn_v(void)
{
	menu_bg_color=46;
	return 0;
}
MODULE_ITEM(item_v,"v:	change menu background color to 青色",e_item,fn_v)

int fn_w(void)
{
	menu_bg_color=47;
	return 0;
}
MODULE_ITEM(item_w,"w:	change menu background color to black",e_item,fn_w)

int fn_x(void)
{
	exit(0);
	return 0;
}
MODULE_ITEM(item_x,"x:	exit.",e_item,fn_x)

int fn_y(void)
{
	menu_bg_color=40;
	menu_color=37;
	return 0;
}
MODULE_ITEM(item_y,"y:	reset menu color to default",e_item,fn_y)

int fn_z(void)
{
	printf("this project is the demo for uboot code arch.\n");
	printf("                 build by Robe 2018.9.4 \n");
	return 0;
}
MODULE_ITEM(item_z,"z:	print project info.",e_item,fn_z)

/* ========================================================菜单列表  */
item_type *menu[]={
	&item_a,
	&item_b,
	&item_c,
	&item_d,
	&item_e,
	&item_f,
	&item_g,
	&item_h,
	&item_i,
	&item_j,
	&item_k,
	&item_l,
	&item_m,
	&item_n,
	&item_o,
	&item_p,
	&item_q,
	&item_r,
	&item_s,
	&item_t,
	&item_u,
	&item_v,
	&item_w,
	&item_x,
	&item_y,
	&item_z,
	NULL,
};

/*=====================================================main */
int array_size(void)
{
	int i;
	for(i=0;;i++)
		if (menu[i]==NULL) break;
	return i;	
}

int init(void)
{
	error_info=1;
	memset(input,0,CHAR_SIZE);
}

int list_menu(void)
{
	printf("\033[0;%d;%dm",menu_color,menu_bg_color);
	system("clear");
	printf("============ MENU: by Robe @2018.9.4 ==========\n");
	for(int i=0;i<MENU_SIZE;i++)
		{
			printf("%s \n",menu[i]->name);
		}
	printf("===============================================");
	printf("\033[0;0m\n");
}

int input_selection(void)
{
	printf("please select the item: ");		
	scanf("%s",input);
}

int action(void)
{
	for(int i=0;i<MENU_SIZE;i++)
	{
		if(input[0]==menu[i]->name[0])
		{
			menu[i]->fn_cmd();
			error_info=0;
		}	
	}
	printf("\n");
	if (error_info==1)
	{
		printf("error:	canot find your select item \n");
		system("echo \"\a\"");
	}
}

int after_action(void)
{
	usleep(1000000);
}

int main(void)
{
	while(1)
	{
		init();
		list_menu();
		input_selection();
		action();
		after_action();
	}
	return 0;
}