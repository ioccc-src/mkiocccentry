#!/usr/bin/make
#
# soup - some delicious IOCCC soup recipes :-)
#


SHELL= /bin/bash

OBJFILES= chk_sem_auth.o chk_sem_info.o chk_validate.o
# This is a simpler way to do:
#
#   SRCFILES =  $(patsubst %.o,%.c,$(OBJFILES))
#
SRCFILES= $(OBJFILES:.o=.c)

all: ${OBJFILES}

chk_sem_auth.o: chk_sem_auth.c chk_sem_auth.h Makefile
	${CC} ${CFLAGS} -I.. chk_sem_auth.c -c

chk_sem_info.o: chk_sem_info.c chk_sem_info.h Makefile
	${CC} ${CFLAGS} -I.. chk_sem_info.c -c

chk_validate.o: chk_validate.c Makefile
	${CC} ${CFLAGS} -I.. chk_validate.c -c


configure:
	@echo nothing to $@. Try cooking instead.

mix mixing mix_ingredients mixing_ingredients:
	@echo You mix the ingredients...

cook cooking: mix
	@echo You cook the soup...

clean:
	${RM} -f *.o

clobber: clean

install: all
	@echo nothing to $@. Try eating the soup instead.

eat eating eat_soup eating_soup: all
	@echo That was mighty delicous soup but now you feel bloated! :-(
