# This Makefile requires GNU make.
SHELL = /bin/sh
CC = gcc
PROGRAM = pl0
OBJDIR = bin
BINDIR = bin
SRCDIR = src
OBJS = ${OBJDIR}/pl0.o
CFLAGS = -m32 -Iinclude
CCOMPILE = ${CC} ${CFLAGS} -c

default:: ${PROGRAM}
	
${PROGRAM}: ${OBJS}
	@mkdir -p ${BINDIR}
	${CC} ${CFLAGS} -o ${BINDIR}/${PROGRAM} ${OBJS}

${OBJDIR}/%.o: ${SRCDIR}/%.c
	@mkdir -p ${OBJDIR}
	${CCOMPILE} -o $@ $<

clean:
	- rm -f ${OBJDIR}/*.o
