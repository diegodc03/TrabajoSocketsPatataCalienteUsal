/*
 *			C L I E N T C P
 *
 *	This is an example program that demonstrates the use of
 *	stream sockets as an IPC mechanism.  This contains the client,
 *	and is intended to operate in conjunction with the server
 *	program.  Together, these two programs
 *	demonstrate many of the features of sockets, as well as good
 *	conventions for using these features.
 *
 *
 */
 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <time.h>


#include "funciones.h"

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
int main(argc, argv)
int argc;
char *argv[];
{
    int s;				/* connected socket descriptor */
   	struct addrinfo hints, *res;
    long timevar;			/* contains time returned by time() */
    struct sockaddr_in myaddr_in;	/* for local socket address */
    struct sockaddr_in servaddr_in;	/* for server socket address */
	int addrlen, i, j, errcode;
    /* This example uses TAM_BUFFER byte messages. */
	char buf[BUFFERSIZE];

	if (argc != 2) {
		fprintf(stderr, "Usage:  %s <remote host>\n", argv[0]);
		exit(1);
	}

	/* Create the socket. */
	s = socket (AF_INET, SOCK_STREAM, 0);
	if (s == -1) {
		perror(argv[0]);
		fprintf(stderr, "%s: unable to create socket\n", argv[0]);
		exit(1);
	}
	
	/* clear out address structures */
	memset ((char *)&myaddr_in, 0, sizeof(struct sockaddr_in));
	memset ((char *)&servaddr_in, 0, sizeof(struct sockaddr_in));

	/* Set up the peer address to which we will connect. */
	servaddr_in.sin_family = AF_INET;
	
	/* Get the host information for the hostname that the
	 * user passed in. */
      memset (&hints, 0, sizeof (hints));
      hints.ai_family = AF_INET;
 	 /* esta funci�n es la recomendada para la compatibilidad con IPv6 gethostbyname queda obsoleta*/
    errcode = getaddrinfo (argv[1], NULL, &hints, &res); 
    if (errcode != 0){
			/* Name was not found.  Return a
			 * special value signifying the error. */
		fprintf(stderr, "%s: No es posible resolver la IP de %s\n",
				argv[0], argv[1]);
		exit(1);
        }
    else {
		/* Copy address of host */
		servaddr_in.sin_addr = ((struct sockaddr_in *) res->ai_addr)->sin_addr;
	    }
    freeaddrinfo(res);

    /* puerto del servidor en orden de red*/
	servaddr_in.sin_port = htons(PUERTO);

		/* Try to connect to the remote server at the address
		 * which was just built into peeraddr.
		 */
	if (connect(s, (const struct sockaddr *)&servaddr_in, sizeof(struct sockaddr_in)) == -1) {
		perror(argv[0]);
		fprintf(stderr, "%s: unable to connect to remote\n", argv[0]);
		exit(1);
	}
		/* Since the connect call assigns a free address
		 * to the local end of this connection, let's use
		 * getsockname to see what it assigned.  Note that
		 * addrlen needs to be passed in as a pointer,
		 * because getsockname returns the actual length
		 * of the address.
		 */
	addrlen = sizeof(struct sockaddr_in);
	if (getsockname(s, (struct sockaddr *)&myaddr_in, &addrlen) == -1) {
		perror(argv[0]);
		fprintf(stderr, "%s: unable to read socket address\n", argv[0]);
		exit(1);
	 }

	/* Print out a startup message for the user. */
	time(&timevar);
	/* The port number must be converted first to host byte
	 * order before printing.  On most hosts, this is not
	 * necessary, but the ntohs() call is included here so
	 * that this program could easily be ported to a host
	 * that does require it.
	 */
	printf("Connected to %s on port %u at %s",
			argv[1], ntohs(myaddr_in.sin_port), (char *) ctime(&timevar));



	
		/* Now, start receiving all of the replys from the server.
		 * This loop will terminate when the recv returns zero,
		 * which is an end-of-file condition.  This will happen
		 * after the server has sent all of its replies, and closed
		 * its end of the connection.
		 */
	char mensaje[BUFFERSIZE];
	//Bucle del juego --> Cuando conteste el cliente ADIOS, PARA DESCONECTARSE CON EL SERVIDOR
	int booleano = 0;
	while(booleano == 0){

		//Recibimos el mensaje.
		// Recibir el mensaje del servidor
        while (i == recv(s, buf, BUFFERSIZE, 0)) {
		if (i == -1) {
            perror(argv[0]);
			fprintf(stderr, "%s: error reading result\n", argv[0]);
			exit(1);
		}
			/* The reason this while loop exists is that there
			 * is a remote possibility of the above recv returning
			 * less than TAM_BUFFER bytes.  This is because a recv returns
			 * as soon as there is some data, and will not wait for
			 * all of the requested data to arrive.  Since TAM_BUFFER bytes
			 * is relatively small compared to the allowed TCP
			 * packet sizes, a partial receive is unlikely.  If
			 * this example had used 2048 bytes requests instead,
			 * a partial receive would be far more likely.
			 * This loop will keep receiving until all TAM_BUFFER bytes
			 * have been received, thus guaranteeing that the
			 * next recv at the top of the loop will start at
			 * the begining of the next reply.
			 */
		while (i < BUFFERSIZE) {
			j = recv(s, &buf[i], BUFFERSIZE-i, 0);
			if (j == -1) {
                     perror(argv[0]);
			         fprintf(stderr, "%s: error reading result\n", argv[0]);
			         exit(1);
               }
			i += j;
		}

		//Escrbimos el mensaje que nos llega.
		printf("El mensaje proviniente del servidor es: \n\t %s", buf);



		//Escribimos el mensaje al servidor.
		if(strcmp(buf, "Cerrando el Servicio")){
			booleano = 1;
			
			//Añadir al log
			if(aniadirAlLog(CLIENTE, mensaje) == -1){
				perror("No se ha podido añadir la respuesta al fichero");	
			}


		}else{
			//No se sale, el cliente debera escribir un mensaje
			scanf("%s", buf);

			//Añadir al log
			if(aniadirAlLog(CLIENTE, mensaje) == -1){
				perror("No se ha podido añadir la respuesta al fichero");	
			}

			//Añadir CRLF
			aniadirCRLF(buf,BUFFERSIZE);

			//Enviar mensaje al servidor
			if (send(s, buf, BUFFERSIZE, 0) != BUFFERSIZE) {
					fprintf(stderr, "%s: Conexión abortada al iniciar el juego\n", argv[0]);
        			exit(1);
				}
		}
	}

	if (shutdown(s, 1) == -1) {
		perror(argv[0]);
		fprintf(stderr, "%s: unable to shutdown socket\n", argv[0]);
		exit(1);
	}

		
	
    /* Print message indicating completion of task. */
	time(&timevar);
	fprintf(mensaje,"All done at %s", (char *)ctime(&timevar));
	if(aniadirAlLog(CLIENTE, mensaje) == -1){
		perror("no se ha podido añadir la respuesta al fichero");
	}
	
}
}





