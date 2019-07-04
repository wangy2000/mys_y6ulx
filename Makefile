
include Makefile2

var= 1 2 3 4 5 1 22 333 4444 55555
var1= 1 2 3 4 5 1 22 333 4444 55555 123 321 
export var var1

.PHONY:all sed bash FORCE

all:
	@echo ${var} " firstword " $(firstword ${var});
	@echo ${var} " fileter " $(filter %,${var});
	@echo ${var} " fileter " $(filter %2,${var});

all:
	@echo " second target. overrideing first target: all.";

makefile2:
	@echo ${var2}

sed:
	echo "robe.zhang" | sed -e s/e./-/
	

ifeq ("$(origin V)", "command line")
endif
	
bash:
	@echo $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
	  else if [ -x /bin/bash ]; then echo /1/bin/bash; \
	  else echo /2/sh; fi ; fi)
	  
make:
	@echo "make : ${MAKE}"

out_makefile:
	@make -f Makefile1 all all_1



	