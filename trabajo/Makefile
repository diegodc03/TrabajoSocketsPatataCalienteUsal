CC = gcc
CFLAGS = -Wall -Wextra -g
#Descomentar la siguiente linea para olivo
#LIBS = -lsocket -lnsl
#Descomentar la siguiente linea para linux
LIBS =

PROGS = servidor clientcp 

all: ${PROGS}

servidor: servidor.o funciones.o
	${CC} ${CFLAGS} -o $@ servidor.o funciones.o 
	
clientcp: clientcp.o funciones.o
	${CC} ${CFLAGS} -o $@ clientcp.o funciones.o 

servidor.o: servidor.c funciones.h
	$(CC) $(CFLAGS) -c servidor.c

clientcp.o: clientcp.c funciones.h
	$(CC) $(CFLAGS) -c clientcp.c

funciones.o: funciones.c funciones.h
	$(CC) $(CFLAGS) -c funciones.c

clean:
	rm *.o ${PROGS}
