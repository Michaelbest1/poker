OBJS = poker_cal.o
CC = gcc
CFLAGS = -Wall -O -g

poker_cal: ${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o poker_cal

poker_cal.o: poker_cal.c poker_cal.h
	${CC} ${CFLAGS} -c poker_cal.c


clean: 
	rm poker_cal
