CC = gcc
CFLAGS = 
# Descomentar la siguiente linea para olivo
# LIBS = -lsocket -lnsl
# Descomentar la siguiente linea para linux
LIBS =

PROGS = servidor clientcp clientudp

all: ${PROGS}

servidor: servidor.o funciones.o
	${CC} ${CFLAGS} -o $@ servidor.o funciones.o ${LIBS}
	
clientcp: clientcp.o funciones.o
	${CC} ${CFLAGS} -o $@ clientcp.o funciones.o ${LIBS}

clientudp: clientudp.o funciones.o
	${CC} ${CFLAGS} -o $@ clientudp.o funciones.o ${LIBS}

funciones.o: funciones.c funciones.h
	${CC} ${CFLAGS} -c funciones.c

clean:
	rm -f *.o ${PROGS}
