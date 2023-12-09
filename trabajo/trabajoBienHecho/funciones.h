
#ifndef __FUNCIONES__
#define __FUNCIONES__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>

//Declaracion de variables
#define RETRIES 5
#define TIMEOUT 6
#define PUERTO 17278
#define ADDRNOTFOUND	0xffffffff	/* return address for unfound host */
#define BUFFERSIZE	516	/* maximum size of packets to be received */
#define TAM_BUFFER 516
#define MAXHOST 128
#define CR '\r'			//Los declaramos aqui para que sea mas facil el llamdo
#define LF '\n'
#define TC '\0'		//Terminacion de Cadena


#define CLIENTE "cliente"
#define SERVIDOR "servidor"
#define HOLA "HOLA"
#define RESPUESTA "RESPUESTA"
#define SIGNOSUMA "+"
#define ADIOS "ADIOS"


extern int errno;




//Funciones
int comprobarMensaje(char *);

int obtenerNumero(char *);

int aniadirAlLog(char *, struct sockaddr_in, char*, char*, int );

int calcularNumeroRandom();

void aniadirCRLF(char *, int );

int eliminarCRLF(char *);

int recibir(int , char *, int , struct sockaddr * , int *);


#endif // !__FUNCIONES__