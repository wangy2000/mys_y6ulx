
#include<stdio.h>
#include<unistd.h>
#include<getopt.h>

int main(int argc, char *argv[])
{
    int opt;
    int digit_optind = 0;
    int option_index = 0;
    char *string = "a::b:c:d";
    static struct option long_options[] =
    {  
        {"reqarg", required_argument,NULL, 'r'},
        {"optarg", optional_argument,NULL, 'o'},
        {"noarg",  no_argument,         NULL,'n'},
        {NULL,     0,                      NULL, 0},
    }; 
    while((opt =getopt_long_only(argc,argv,string,long_options,&option_index))!= -1)
    {  
        printf("opt = %c\t\t", opt);
        printf("optarg = %s\t\t",optarg);
        printf("optind = %d\t\t",optind);
        printf("argv[optind] =%s\t\t", argv[optind]);
        printf("option_index = %d\n",option_index);
    }  
}

#if 0
has_arg  指明是否带参数值，其数值可选：
no_argument         表明长选项不带参数，如：--name, --help
required_argument  表明长选项必须带参数，如：--prefix /root或 --prefix=/root
optional_argument  表明长选项的参数是可选的，如：--help或 –prefix=/root，其它都是错误

struct option {
const char  *name;       /* 参数名称 */
int          has_arg;    /* 指明是否带有参数 */
int          *flag;      /* flag=NULL时,返回value;不为空时,*flag=val,返回0 */
int          val;        /* 用于指定函数找到选项的返回值或flag非空时指定*flag的值 */
}; 
#endif



