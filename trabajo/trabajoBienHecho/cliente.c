/*
** Fichero: cliente.c
** Autores:
** DIEGO DE CASTRO MERILLAS DNI 71043687e
** 
*/

/*
 *			M A I N
 *
 *	This routine is the client which request service from the remote.
 *	It creates a connection, sends a number of
 *	requests, shuts down the connection in one direction to signal the
 *	server about the end of data, and then receives all of the responses.
 *	Status will be written to stdout.
 *
 *	The name of the system to which the requests will be sent is given
 *	as a parameter to the command.
 */

/*
Funcion donde se elige si se quiere clienteUDP o clienTCP, por los apramtetros
*/


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

extern int errno;
#include "clientcp.h"
#include "clientudp.h"

int argc;
char **argv;

int main(int argc, char **argv)
{

    if(argc != 3 && argc != 4){
        printf("<%s> <host> <Protocol> --> argc = %d\n",argv[0], argc);
        printf("<%s> <host> <Protocol> <fichero> --> argc = %d\n",argv[0], argc);
    }

    if(strcmp(argv[2], "TCP") == 0){
        //printf("Estoy en TCP");
        clienTCP(argv, argc);
    }else if(strcmp(argv[2], "UDP") == 0){
        //printf("Estoy en UDP");
        clientUDP(argv, argc);
    }

    return 1;
}
	

