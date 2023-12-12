
#ifndef __FUNCIONES__
#define __FUNCIONES__

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <netdb.h>
#include <string.h>
#include <time.h>

#include <unistd.h>

//Declaracion de variables
#define RETRIES 5
#define TIMEOUT 6
#define PUERTO 39004

#define ADDRNOTFOUND	0xffffffff	/* return address for unfound host */
#define BUFFERSIZE	516	/* maximum size of packets to be received */
#define TAM_BUFFER 516
#define MAXHOST 128
#define CR '\r'			//Los declaramos aqui para que sea mas facil el llamdo
#define LF '\n'
#define TC '\0'		//Terminacion de Cadena




extern int errno;




//Funciones
int comprobarMensaje(char *);

int obtenerNumero(char *);

int aniadirAlLog(char *, struct sockaddr_in, char*, char*, int, unsigned int);

int calcularNumeroRandom();

int eliminarCRLF(char *);

int recibir(int , char *, int , struct sockaddr * , int *);


#endif // !__FUNCIONES__