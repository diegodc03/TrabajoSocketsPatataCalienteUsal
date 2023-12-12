CC = gcc
CFLAGS = 
LIBS =

# Aquí se enumeran todos los ejecutables a construir
PROGS	=	cliente	servidor

all:	$(PROGS)


# Regla para compilar cliente.c a cliente.o
cliente.o:	cliente.c
	$(CC)	$(CFLAGS)	-c	cliente.c

# Regla para compilar clientcp.c a clientcp.o
clientcp.o:	clientcp.c
	$(CC)	$(CFLAGS)	-c	clientcp.c

# Regla para compilar clientudp.c a clientudp.o
clientudp.o:	clientudp.c
	$(CC)	$(CFLAGS)	-c	clientudp.c

# Regla para compilar funciones.c a funciones.o
funciones.o:	funciones.c	funciones.h
	$(CC)	$(CFLAGS)	-c	funciones.c

servidor:	servidor.o	funciones.o
	${CC}	${CFLAGS}	-o	$@	servidor.o	funciones.o	${LIBS}

# Regla para construir el ejecutable cliente
cliente:	cliente.o	clientcp.o	clientudp.o	funciones.o
	$(CC)	$(CFLAGS)	-o	$@	cliente.o	clientcp.o	clientudp.o	funciones.o	$(LIBS)


# Regla para limpiar los archivos compilados
clean:
	rm -f *.o $(PROGS)