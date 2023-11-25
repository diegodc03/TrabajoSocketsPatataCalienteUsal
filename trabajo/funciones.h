
#ifndef __FUNCIONES__
#define __FUNCIONES__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//Declaracion de variables

#define PUERTO 17278
#define ADDRNOTFOUND	0xffffffff	/* return address for unfound host */
#define BUFFERSIZE	1024	/* maximum size of packets to be received */
#define TAM_BUFFER 10
#define MAXHOST 128
#define CR '\r'			//Los declaramos aqui para que sea mas facil el llamdo
#define LF '\n'
#define TC '\0'		//Terminacion de Cadena


#define CLIENTE 'cliente'
#define SERVIDOR 'servidor'
#define HOLA 'HOLA'
#define RESPUESTA 'RESPUESTA'
#define SIGNOSUMA '+'
#define ADIOS 'ADIOS'


extern int errno;


//Struct

typedef struct{
	char mensaje[TAM_BUFFER];
	int numero;
}Msj;


typedef struct{
	int num;
	char cadena[BUFFERSIZE];
}respuestasServidor;




//Funciones
int removerCRLF(char *);
void aniadirCRLF(char *, int );
int comprobacionMensaje(char *);
Msj pasamosMensaje(char *);
int calcularNumeroRandom();
int aniadirAlLog(char *, char*);



#endif // !__FUNCIONES__