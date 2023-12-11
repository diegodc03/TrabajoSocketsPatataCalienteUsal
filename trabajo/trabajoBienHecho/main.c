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
#include "funciones.h"
//#include "clientcp.c"
//#include "clientudp.c"

int main(argc, argv)
int argc;
char *argv[];
{

    //En este ejercicio con los parámetros del sistema se ejecutara de una manera u otra
    if(argc < 2)
    {
        printf("Argumentos");
    }
    else if(argc == 2 ){
        //Nos han puesto tipo parametro
        //Tiene los argumetos para llamar a UDP
        if(strcmp(argv[2], "TCP")){
                printf("f");
        }else if(strcmp(argv[2], "UDP")){
            printf("f");
        }else{
            exit(1);
        }


    }else if(argc ==3){
        printf("f");
    }else{
        exit(1);
    }

    return 1;


}
	

