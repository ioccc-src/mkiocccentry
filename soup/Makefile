#!/usr/bin/make
#
# soup - some delicious IOCCC soup recipes :-)
#


SHELL= /bin/bash
ECHO = echo

OBJFILES= chk_sem_auth.o chk_sem_info.o chk_validate.o
# This is a simpler way to do:
#
#   SRCFILES =  $(patsubst %.o,%.c,$(OBJFILES))
#
SRCFILES= $(OBJFILES:.o=.c)

all: ${OBJFILES}

.PHONY: all

all:
	@:

chk_sem_auth.o: chk_sem_auth.c chk_sem_auth.h Makefile
	${CC} ${CFLAGS} -I.. chk_sem_auth.c -c

chk_sem_info.o: chk_sem_info.c chk_sem_info.h Makefile
	${CC} ${CFLAGS} -I.. chk_sem_info.c -c

chk_validate.o: chk_validate.c Makefile
	${CC} ${CFLAGS} -I.. chk_validate.c -c


configure:
	@echo "nothing to $@. Try making some soup instead."

soup:
	@-if [[ -r .soup ]]; then \
	    ${ECHO} "Soup already cooked. Don't forget to eat it before it goes cold!"; \
        else \
	    ${ECHO} "Mixing the ingredients..."; \
	    ${ECHO} "Cooking the soup..."; \
	    touch .soup; \
	    sleep 2; \
	    ${ECHO} "Soup made. Don't forget to eat it before it goes cold!"; \
	fi

clean:
	${RM} *.o

clobber: clean
	${RM} .soup

install: all
	@${ECHO} "nothing to $@. Try eating the soup instead."

eat eating eat_soup eating_soup: all
	@-if [[ -r .soup ]]; then \
	    ${ECHO} "That was mighty delicous soup but now you feel bloated! :-("; \
	    rm -f .soup; \
	else \
	    ${ECHO} "How can you eat soup that you haven't made yet?"; \
	fi
