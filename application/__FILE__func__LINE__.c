
#include<stdio.h>
#include<unistd.h>
#include<getopt.h>

enum conf_def_mode {
	def_default,
	def_yes,
	def_mod,
	def_no,
	def_random
};


int main(int argc, char *argv[])
{
	printf("__%d__\n",def_default);
	printf("file:%s . func:%s . line:%d . \n",__FILE__,__func__,__LINE__);
	return 0;
}





