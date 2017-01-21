CC=gcc
CFLAGS=-Wall -std=gnu11 -g

all: assembler

lineList.o : lineList.c parser.h
	${CC} ${CFLAGS} $< -c

replaceSymbols.o: replaceSymbols.c parser.h
	${CC} ${CFLAGS} $< -c

lineToMachineCode.o: lineToMachineCode.c parser.h
	${CC} ${CFLAGS} $< -c

assembler : assembler.c parser.o lineList.o replaceSymbols.o lineToMachineCode.o
	${CC} ${CFLAGS} $^ -o $@

clean:
	rm -f *.o assembler
