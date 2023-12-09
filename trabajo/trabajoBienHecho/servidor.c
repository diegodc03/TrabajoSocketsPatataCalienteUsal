

/*
 *          		S E R V I D O R
 *
 *	This is an example program that demonstrates the use of
 *	sockets TCP and UDP as an IPC mechanism.  
 *
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "funciones.h"



	
extern int errno;

/*
int comprobarMensaje(char *);
int obtenerNumero(char *);
int aniadirAlLog(char *, struct sockaddr_in, char*, char*, int );
int calcularNumeroRandom();
void aniadirCRLF(char *, int );
int eliminarCRLF(char *string);
*/
/*
 *			M A I N
 *
 *	This routine starts the server.  It forks, leaving the child
 *	to do all the work, so it does not have to be run in the
 *	background.  It sets up the sockets.  It
 *	will loop forever, until killed by a signal.
 *
 */
 
void serverTCP(int s, struct sockaddr_in peeraddr_in, struct sockaddr_in);
void serverUDP(int s, struct sockaddr_in clientaddr_in);
void errout(char *);		/* declare error out routine */

int FIN = 0;             /* Para el cierre ordenado */
void finalizar(){ FIN = 1; }

int main(argc, argv)
int argc;
char *argv[];
{

    int s_TCP, s_UDP;		/* connected socket descriptor */
    int ls_TCP, ls_UDP;				/* listen socket descriptor */
    
    int cc;				    /* contains the number of bytes read */
     
    struct sigaction sa = {.sa_handler = SIG_IGN}; /* used to ignore SIGCHLD */
    
    struct sockaddr_in myaddr_in;	/* for local socket address */
    struct sockaddr_in clientaddr_in;	/* for peer socket address */
	int addrlen;
	
    fd_set readmask;
    int numfds,s_mayor;
    
    char buffer[BUFFERSIZE];	/* buffer for packets to be read into */
    
    struct sigaction vec;

		/* Create the listen socket. */
	ls_TCP = socket (AF_INET, SOCK_STREAM, 0);
	if (ls_TCP == -1) {
		perror(argv[0]);
		fprintf(stderr, "%s: unable to create socket TCP\n", argv[0]);
		exit(1);
	}
	/* clear out address structures */
	memset ((char *)&myaddr_in, 0, sizeof(struct sockaddr_in));
   	memset ((char *)&clientaddr_in, 0, sizeof(struct sockaddr_in));

    addrlen = sizeof(struct sockaddr_in);

		/* Set up address structure for the listen socket. */
	myaddr_in.sin_family = AF_INET;
		/* The server should listen on the wildcard address,
		 * rather than its own internet address.  This is
		 * generally good practice for servers, because on
		 * systems which are connected to more than one
		 * network at once will be able to have one server
		 * listening on all networks at once.  Even when the
		 * host is connected to only one network, this is good
		 * practice, because it makes the server program more
		 * portable.
		 */
	//myaddr_in.sin_port = 0;
	myaddr_in.sin_addr.s_addr = INADDR_ANY;
	myaddr_in.sin_port = htons(PUERTO);

	/* Bind the listen address to the socket. */
	if (bind(ls_TCP, (const struct sockaddr *) &myaddr_in, sizeof(struct sockaddr_in)) == -1) {
		perror(argv[0]);
		fprintf(stderr, "%s: unable to bind address TCP\n", argv[0]);
		exit(1);
	}
		/* Initiate the listen on the socket so remote users
		 * can connect.  The listen backlog is set to 5, which
		 * is the largest currently supported.
		 */

	

	if (listen(ls_TCP, 5) == -1) {
		perror(argv[0]);
		fprintf(stderr, "%s: unable to listen on socket\n", argv[0]);
		exit(1);
	}
	
	


	//////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////
	
	
	
	/* Create the socket UDP. */
	ls_UDP = socket (AF_INET, SOCK_DGRAM, 0);
	if (ls_UDP == -1) {
		perror(argv[0]);
		printf("%s: unable to create socket UDP\n", argv[0]);
		exit(1);
	   }
	/* Bind the server's address to the socket. */
	if (bind(ls_UDP, (struct sockaddr *) &myaddr_in, sizeof(struct sockaddr_in)) == -1) {
		perror(argv[0]);
		printf("%s: unable to bind address UDP\n", argv[0]);
		exit(1);
	    }



		/* Now, all the initialization of the server is
		 * complete, and any user errors will have already
		 * been detected.  Now we can fork the daemon and
		 * return to the user.  We need to do a setpgrp
		 * so that the daemon will no longer be associated
		 * with the user's control terminal.  This is done
		 * before the fork, so that the child will not be
		 * a process group leader.  Otherwise, if the child
		 * were to open a terminal, it would become associated
		 * with that terminal as its control terminal.  It is
		 * always best for the parent to do the setpgrp.
		 */
	setpgrp();

	switch (fork()) {
	case -1:		/* Unable to fork, for some reason. */
		perror(argv[0]);
		printf("%s: unable to fork daemon\n", argv[0]);
		exit(1);

	case 0:     /* The child process (daemon) comes here. */

			/* Close stdin and stderr so that they will not
			 * be kept open.  Stdout is assumed to have been
			 * redirected to some logging file, or /dev/null.
			 * From now on, the daemon will not report any
			 * error messages.  This daemon will loop forever,
			 * waiting for connections and forking a child
			 * server to handle each one.
			 */
		fclose(stdin);
		fclose(stderr);

			/* Set SIGCLD to SIG_IGN, in order to prevent
			 * the accumulation of zombies as each child
			 * terminates.  This means the daemon does not
			 * have to make wait calls to clean them up.
			 */
		if ( sigaction(SIGCHLD, &sa, NULL) == -1) {
            perror(" sigaction(SIGCHLD)");
            fprintf(stderr,"%s: unable to register the SIGCHLD signal\n", argv[0]);
            exit(1);
            }
            
		    /* Registrar SIGTERM para la finalizacion ordenada del programa servidor */
        vec.sa_handler = (void *) finalizar;
        vec.sa_flags = 0;
        if ( sigaction(SIGTERM, &vec, (struct sigaction *) 0) == -1) {
            perror(" sigaction(SIGTERM)");
            fprintf(stderr,"%s: unable to register the SIGTERM signal\n", argv[0]);
            exit(1);
            }
        
		while (!FIN) {
            /* Meter en el conjunto de sockets los sockets UDP y TCP */
            FD_ZERO(&readmask);
            FD_SET(ls_TCP, &readmask);
            FD_SET(ls_UDP, &readmask);
            /* 
            Seleccionar el descriptor del socket que ha cambiado. Deja una marca en 
            el conjunto de sockets (readmask)
            */ 
    	    if (ls_TCP > ls_UDP) s_mayor=ls_TCP;
    		else s_mayor=ls_UDP;

            if ( (numfds = select(s_mayor+1, &readmask, (fd_set *)0, (fd_set *)0, NULL)) < 0) {
                if (errno == EINTR) {
                    FIN=1;
		            close (ls_TCP);
		            close (ls_UDP);
                    perror("\nFinalizando el servidor. Se�al recibida en elect\n "); 
                }
            }
           else { 

                /* Comprobamos si el socket seleccionado es el socket TCP */
                if (FD_ISSET(ls_TCP, &readmask)) {
                    /* Note that addrlen is passed as a pointer
                     * so that the accept call can return the
                     * size of the returned address.
                     */
    				/* This call will block until a new
    				 * connection arrives.  Then, it will
    				 * return the address of the connecting
    				 * peer, and a new socket descriptor, s,
    				 * for that connection.
    				 */
    				s_TCP = accept(ls_TCP, (struct sockaddr *) &clientaddr_in, &addrlen);
    				if (s_TCP == -1) 
						exit(1);
    			
					switch (fork()) {
        				case -1:	/* Can't fork, just exit. */
        					exit(1);
        				case 0:		/* Child process comes here. */
                			close(ls_TCP); /* Close the listen socket inherited from the daemon. */
        					serverTCP(s_TCP, clientaddr_in, myaddr_in);
        					exit(0);
        				default:	/* Daemon process comes here. */
        					/* The daemon needs to remember
        					 * to close the new accept socket
        					 * after forking the child.  This
        					 * prevents the daemon from running
        					 * out of file descriptor space.  It
        					 * also means that when the server
        					 * closes the socket, that it will
        					 * allow the socket to be destroyed
        					 * since it will be the last close.
        					 */
        					close(s_TCP);
        			}
             	} /* De TCP*/

          
		  
		  		/* Comprobamos si el socket seleccionado es el socket UDP */
          		if (FD_ISSET(ls_UDP, &readmask)) {

                	/* This call will block until a new
                	* request arrives.  Then, it will
                	* return the address of the client,
                	* and a buffer containing its request.
                	* BUFFERSIZE - 1 bytes are read so that
                	* room is left at the end of the buffer
                	*	 for a null character.
                	*/
				

					int numeroIntentosRec = 0;

					while(numeroIntentosRec < RETRIES){
						alarm(TIMEOUT);
						cc = recvfrom(ls_UDP, buffer, BUFFERSIZE, 0,
                   			(struct sockaddr *)&clientaddr_in, &addrlen);
    		
						if ( cc == -1) {
							numeroIntentosRec = numeroIntentosRec + 1;
        					printf(": recvfrom error\n");
        					//exit (1);
							if(numeroIntentosRec == RETRIES){
								exit(2);
							}


        				}else{
							alarm(0);
							break;
						}

					}

					//printf("%s",buffer);

					/* When a new client sends a UDP datagram, his information is stored
					* in "clientaddr_in", so we can create a false connection by sending messages
					* manually with this information
					*/
					s_UDP = socket(AF_INET, SOCK_DGRAM, 0);
					if (s_UDP == -1) {
						perror(argv[0]);
						printf("%s: unable to create new socket UDP for new client\n", argv[0]);
						exit(1);
					}

					/* Clear and set up address structure for new socket. 
					* Port 0 is specified to get any of the avaible ones, as well as the IP address.
					*/						
					memset ((char *)&myaddr_in, 0, sizeof(struct sockaddr_in));
					myaddr_in.sin_family = AF_INET;
					myaddr_in.sin_addr.s_addr = INADDR_ANY;
					myaddr_in.sin_port = htons(0);
						
					/* Bind the server's address to the new socket for the client. */
					if (bind(s_UDP, (struct sockaddr *) &myaddr_in, sizeof(struct sockaddr_in)) == -1) {
						perror(argv[0]);
						printf("%s: unable to bind address new socket UDP for new client\n", argv[0]);
						exit(1);
					}


					/* As well as its done in TCP, a new thread is created for that false connection */
					switch (fork()) {
						case -1:	
							exit(1);
							
						case 0:		/* Child process comes here. */
									/* Child doesnt need the listening socket */
				    		close(ls_UDP); 
				    										
							serverUDP(s_UDP, clientaddr_in);
							exit(0);
							
						default:
							close(s_UDP);
						}





                	/* Make sure the message received is
                	* null terminated.
                	*/
                	//buffer[cc]='\0';
                	//serverUDP (s_UDP, buffer, clientaddr_in);
                }
          	}
		}   
		
		
		//ENTENDEMOS QUE EL SERVER UDP, cuando termina llega aqui
		/* Fin del bucle infinito de atenci�n a clientes */
        /* Cerramos los sockets UDP y TCP */
        close(ls_TCP);
        close(s_UDP);
    
        printf("\nFin de programa servidor!\n");
        
	default:		/* Parent process comes here. */
		exit(0);
	}

}


/*
// Abre un archivo para escritura
    			file = fopen("salida.txt", "w");
    			if (file == NULL) {
        		perror("Error al abrir el archivo");
        		return 1;
    			}

    			// Escribe en el archivo
    			fprintf(file, "eSTOY ANTES DE LLEER.\n");

    			// Cierra el archivo
    			fclose(file);
		
				
*/
//////////////////////////////// FIN DE MAIN ////////////////////////////////////////////////////////////////////////////











/*
 *				S E R V E R T C P
 *
 *	This is the actual server routine that the daemon forks to
 *	handle each individual connection.  Its purpose is to receive
 *	the request packets from the remote client, process them,
 *	and return the results to the client.  It will also write some
 *	logging information to stdout.
 *
 */
void serverTCP(int s, struct sockaddr_in clientaddr_in, struct sockaddr_in seraddr_in)
{
	


	int reqcnt = 0;		/* keeps count of number of requests */
	char buf[TAM_BUFFER];		/* This example uses TAM_BUFFER byte messages. */
	char hostname[MAXHOST];		/* remote host's name string */
	char auxFich[TAM_BUFFER];
	int len, len1, status;
    struct hostent *hp;		/* pointer to host info for remote host */
    long timevar;			/* contains time returned by time() */
    
    struct linger linger;		/* allow a lingering, graceful close; */
    				            /* used when setting SO_LINGER */
    				
	/* Look up the host information for the remote host
	 * that we have connected with.  Its internet address
	 * was returned by the accept call, in the main
	 * daemon loop above.
	 */
	 
     status = getnameinfo((struct sockaddr *)&clientaddr_in,sizeof(clientaddr_in),
                           hostname,MAXHOST,NULL,0,0);
     if(status){
           	/* The information is unavailable for the remote
			 * host.  Just format its internet address to be
			 * printed out in the logging information.  The
			 * address will be shown in "internet dot format".
			 */
			 /* inet_ntop para interoperatividad con IPv6 */
            if (inet_ntop(AF_INET, &(clientaddr_in.sin_addr), hostname, MAXHOST) == NULL)
            	perror(" inet_ntop \n");
             }
    /* Log a startup message. */
    time (&timevar);
		/* The port number must be converted first to host byte
		 * order before printing.  On most hosts, this is not
		 * necessary, but the ntohs() call is included here so
		 * that this program could easily be ported to a host
		 * that does require it.
		 */
	printf("Startup from %s port %u at %s",
		hostname, ntohs(clientaddr_in.sin_port), (char *) ctime(&timevar));

		/* Set the socket for a lingering, graceful close.
		 * This will cause a final close of this socket to wait until all of the
		 * data sent on it has been received by the remote host.
		 */
	linger.l_onoff  =1;
	linger.l_linger =1;
	if (setsockopt(s, SOL_SOCKET, SO_LINGER, &linger,
					sizeof(linger)) == -1) {
		errout(hostname);
	}

		/* Go into a loop, receiving requests from the remote
		 * client.  After the client has sent the last request,
		 * it will do a shutdown for sending, which will cause
		 * an end-of-file condition to appear on this end of the
		 * connection.  After all of the client's requests have
		 * been received, the next recv call will return zero
		 * bytes, signalling an end-of-file condition.  This is
		 * how the server will know that no more requests will
		 * follow, and the loop will be exited.
		 */

	//Finalizar el bucle ya que quiere terminar xd
	int finalizacion = 0;
	int i=0;
	//Declaro variables para ver si se puede entrar en los casos o no
	int mensajeHola = 0; //En el momento que entre en Hola ya no podria entrar mas, es el primer mensaje del servidor
	int estamosJugando = 0;
	int primerMensaje = 0;
	int tipo;
	int numero = 0;
	int aux= 0;
	int numIntentos;
	int valorAResolver;
	
	//Primera cosa que se hace, añadir al log
	//if(aniadirAlLog("", 0) == -1){
	//			perror("No se ha podido añadir la respuesta al fichero");
	//}

	sprintf(auxFich,"Startup from %s port %u at %s",
		hostname, ntohs(clientaddr_in.sin_port), (char *) ctime(&timevar));

	//if(aniadirAlLog(auxFich, 1) == -1){
	//			perror("No se ha podido añadir la respuesta al fichero");
	//}
	

	if (send(s, "220 Servicio Preparado\r\n", TAM_BUFFER, 0) != TAM_BUFFER) errout(hostname);
	//Añadir al log	
	if(aniadirAlLog("220 Servicio Preparado", clientaddr_in, hostname, "TCP", 0) == -1){
		perror("No se ha podido añadir la respuesta al fichero");
	}

	while (len = recv(s, buf, TAM_BUFFER, 0)) {
		if (len == -1) errout(hostname); /* error from recv */
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
			 * the begining of the next request.
			 */
		while (len < TAM_BUFFER) {
			len1 = recv(s, &buf[len], TAM_BUFFER-len, 0);
			if (len1 == -1) errout(hostname);
			len += len1;
		}
			
		
			/* Increment the request count. */
		reqcnt++;
			/* This sleep simulates the processing of the
			 * request that a real server might do.
			 */
		sleep(2);

		//Se elimina \r\n
		aux = eliminarCRLF(buf);
		


		//Empezamos funcionalidad del programa
		tipo = comprobarMensaje(buf);
		//printf("\n%d\n",tipo);
		
		//Comprobamos si es tipo = 2, ya que tendria una respuesta y son dos mensjes a tener
		if(tipo == 2){
			numero = obtenerNumero(buf);
			//printf("numero %d",numero);
			if(numero <= -1){
				tipo = 5;
			}
		}



		//DEPENDIENDO EL CLIENTE, SE DEVOLVERÁ UNA COSA U OTRA

		//HOLA
		if(tipo == 1 && mensajeHola == 0){
			valorAResolver = calcularNumeroRandom();		//Numero random entre 2 valores, en este caso, 0 y 100
			numIntentos = 4;
			char numStr[10];
			char mensaje[TAM_BUFFER];

			strcpy(mensaje,"250 Adivina el valor entre 0 y 100#");
			sprintf(numStr, "%d",numIntentos);
			strcat(mensaje, numStr);
			estamosJugando = 1;
			mensajeHola = 1;

			
			//Añadir al log			
			if(aniadirAlLog(mensaje, seraddr_in, hostname, "TCP",0) == -1){
				perror("No se ha podido añadir la respuesta al fichero");
			}

			strcat(mensaje, "\r\n");
			if (send(s, mensaje, TAM_BUFFER, 0) != TAM_BUFFER) {
				errout(hostname);
			}
		}
		//RESPUESTA <NUMERO>
		else if(tipo == 2 && estamosJugando == 1){
			char numStr[TAM_BUFFER];
			char mensaje[TAM_BUFFER];
			i = 0;
			numIntentos = numIntentos - 1;
			//printf("\n%d\t", valorAResolver);
			printf("\n%d\t", numero);
			//Tiene que enviar si es mayor, menor y el numero de intentos restantes
			//Si se acierta se enviará una respuesta de aviso --> Tenemos un struct en el que esta el mensaje y el numero separados

			if(numIntentos == 0){
				strcpy(mensaje, "375 FALLO");
				estamosJugando = 0;
			}else{

				if(numero > valorAResolver){
					//Es MENOR EL NUMERO				
					strcpy(mensaje, "354 MENOR#");
					sprintf(numStr, "%d",numIntentos);
					strcat(mensaje,numStr);

				}else if(numero < valorAResolver){
					//EL NUMERO ES MAYOR
					strcpy(mensaje, "354 MAYOR#");
					sprintf(numStr, "%d",numIntentos);
					strcat(mensaje,numStr);
				}else {

					//ACIERTO
					i=1;
					strcpy(mensaje, "350 ACIERTO#");
					sprintf(numStr, "%d",numIntentos);
					strcat(mensaje,numStr);
					estamosJugando = 0;
					}
			}

			//Añadir al log		
			if(aniadirAlLog(mensaje, seraddr_in, hostname, "TCP",0) == -1){
				perror("No se ha podido añadir la respuesta al fichero");
			}

			aniadirCRLF(mensaje, TAM_BUFFER);
			if (send(s, mensaje, TAM_BUFFER, 0) != TAM_BUFFER ) {
				errout(hostname);
			}
			

		}
		//Cliente "+"
		else if(tipo == 3 && (estamosJugando == 0 || numIntentos == 0) && mensajeHola == 1){	//Solo se mete si numero de errores = 0 ooooo hemos terminado){
			char mensaje[TAM_BUFFER];
			valorAResolver = calcularNumeroRandom();		//Numero random entre 2 valores, en este caso, 0 y 100
			numIntentos = 5;
			estamosJugando = 1;
			char numStr[10];

			strcpy(mensaje, "250 Adivina el valor entre 0 y 100#");
			sprintf(numStr, "%d",numIntentos);
			strcat(mensaje, numStr);

				
			//Añadir al log		
			if(aniadirAlLog(mensaje, seraddr_in, hostname, "TCP", 0) == -1){
				perror("No se ha podido añadir la respuesta al fichero");
			}
					
			aniadirCRLF(mensaje, TAM_BUFFER);
			if (send(s, mensaje, TAM_BUFFER, 0) != TAM_BUFFER) {
				errout(hostname);
			}
			
		}
		//Cliente "ADIOS"
		else if(tipo == 4){
			char mensaje[TAM_BUFFER];
			strcpy(mensaje, "221 Cerrando el Servicio");

			//Añadir al log			
			if(aniadirAlLog(mensaje, seraddr_in, hostname, "TCP", 0) == -1){
				perror("No se ha podido añadir la respuesta al fichero");
			}
			
			aniadirCRLF(mensaje, TAM_BUFFER);
			if (send(s, mensaje, TAM_BUFFER, 0) != TAM_BUFFER) {
				errout(hostname);
			}

			finalizacion = 1;
			
		}
		// Cliente ESCRIBE MAL LA RESPUESTA, DEVUELVE ERROR DE SINTAXIS
		else{
			
			char mensaje[TAM_BUFFER];
			strcpy(mensaje, "500 Error de sintaxis");

			//Añadir al log		
			if(aniadirAlLog(mensaje, seraddr_in, hostname, "TCP", 0) == -1){
				perror("No se ha podido añadir la respuesta al fichero");
			}

			aniadirCRLF(mensaje, TAM_BUFFER);
			if(send(s, mensaje, TAM_BUFFER, 0) != TAM_BUFFER){
				errout(hostname);
			}
		}



			/* Send a response back to the client. */
			//strcat(buf, "\r\n");
		///if (send(s, buf, TAM_BUFFER, 0) != TAM_BUFFER) errout(hostname);
		if(finalizacion == 1){
			break;
		}
	
	}
		/* The loop has terminated, because there are no
		 * more requests to be serviced.  As mentioned above,
		 * this close will block until all of the sent replies
		 * have been received by the remote host.  The reason
		 * for lingering on the close is so that the server will
		 * have a better idea of when the remote has picked up
		 * all of the data.  This will allow the start and finish
		 * times printed in the log file to reflect more accurately
		 * the length of time this connection was used.
		 */
		close(s);

		/* Log a finishing message. */
		time (&timevar);
		/* The port number must be converted first to host byte
		 * order before printing.  On most hosts, this is not
		 * necessary, but the ntohs() call is included here so
		 * that this program could easily be ported to a host
		 * that does require it.
		 */
		printf("Completed %s port %u, %d requests, at %s\n",
		hostname, ntohs(clientaddr_in.sin_port), reqcnt, (char *) ctime(&timevar));
}

/*
 *	This routine aborts the child process attending the client.
 */
void errout(char *hostname)
{
	printf("Connection with %s aborted on error\n", hostname);
	exit(1);     
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77









/*
 *				S E R V E R U D P
 *
 *	This is the actual server routine that the daemon forks to
 *	handle each individual connection.  Its purpose is to receive
 *	the request packets from the remote client, process them,
 *	and return the results to the client.  It will also write some
 *	logging information to stdout.
 *
 */
void serverUDP(int s, struct sockaddr_in clientaddr_in)
{
    struct in_addr reqaddr;	/* for requested host's address */
    struct hostent *hp;		/* pointer to host info for requested host */
    int nc, errcode;
	char buf[BUFFERSIZE];
    struct addrinfo hints, *res;
	int cc;
	int addrlen;
	int status;
    char hostname[MAXHOST];
   	addrlen = sizeof(struct sockaddr_in);
	long timevar;
	int rqnct = 0;

	//Declaraciones para hacer la funcionalidad
	//Finalizar el bucle ya que quiere terminar xd
	int finalizacion = 0;
	int i=0;
	//Declaro variables para ver si se puede entrar en los casos o no
	int mensajeHola = 0; //En el momento que entre en Hola ya no podria entrar mas, es el primer mensaje del servidor
	int estamosJugando = 0;
	int primerMensaje = 0;
	int tipo;
	int numero = 0;
	int aux= 0;
	int numIntentos;
	int valorAResolver;
	//char mensaje[BUFFERSIZE];

	status = getnameinfo((struct sockaddr *)&clientaddr_in,sizeof(clientaddr_in), hostname, MAXHOST,NULL,0,0);
	if(status){
		/* The information is unavailable for the remote
			 * host.  Just format its internet address to be
			 * printed out in the logging information.  The
			 * address will be shown in "internet dot format".
			 */
		 /* inet_ntop para interoperatividad con IPv6 */
		if (inet_ntop(AF_INET, &(clientaddr_in.sin_addr), hostname, MAXHOST) == NULL)
			perror(" inet_ntop \n");
	}
	//printf("adads");

	//Enviamos Primer mensaje al cliente
	if ( nc = sendto (s, "220 Servicio Preparado\r\n", BUFFERSIZE,
			0, (struct sockaddr *)&clientaddr_in, addrlen) == -1) {
        perror("serverUDP");
        printf("%s: sendto error\n", "serverUDP");
        return;
    }
	//Añadir al log		
	if(aniadirAlLog("220 Servicio Preparado", clientaddr_in, hostname, "UDP", 0) == -1){
		perror("No se ha podido añadir la respuesta al fichero");
	}


	int numeroIntentosRec;
	while(1){
		

		char buf[BUFFERSIZE];
		//Recibe la respuesta y despues el servidor hace lo necesario
		numeroIntentosRec = 0;

		while(numeroIntentosRec < RETRIES){
			alarm(TIMEOUT);
			cc = recvfrom(s, buf, BUFFERSIZE, 0,
                   		(struct sockaddr *)&clientaddr_in, &addrlen);
    		
			if ( cc == -1) {
				numeroIntentosRec = numeroIntentosRec + 1;
        		printf(": recvfrom error\n");
        		//exit (1);
				if(numeroIntentosRec == RETRIES){
					exit(2);
				}

				
        	}else{
				alarm(0);
				break;
			}

		}
		



		sleep(2);

		//Se elimina \r\n
		aux = eliminarCRLF(buf);
		


		//Empezamos funcionalidad del programa
		tipo = comprobarMensaje(buf);
		//printf("\n%d\n",tipo);
		
		//Comprobamos si es tipo = 2, ya que tendria una respuesta y son dos mensjes a tener
		if(tipo == 2){
			numero = obtenerNumero(buf);
			//printf("numero %d",numero);
			if(numero <= -1){
				tipo = 5;
			}
		}



		//DEPENDIENDO EL CLIENTE, SE DEVOLVERÁ UNA COSA U OTRA

		//HOLA
		if(tipo == 1 && mensajeHola == 0){
			valorAResolver = calcularNumeroRandom();		//Numero random entre 2 valores, en este caso, 0 y 100
			numIntentos = 4;
			char numStr[10];
			char mensaje[BUFFERSIZE];

			strcpy(mensaje,"250 Adivina el valor entre 0 y 100#");
			sprintf(numStr, "%d",numIntentos);
			strcat(mensaje, numStr);
			estamosJugando = 1;
			mensajeHola = 1;

			
			//Añadir al log		
			if(aniadirAlLog(mensaje, clientaddr_in, hostname, "UDP", 0) == -1){
				perror("No se ha podido añadir la respuesta al fichero");
			}
			//aniadirCRLF(mensaje, BUFFERSIZE);
			
			strcat(mensaje, "\r\n");
			if ( nc = sendto (s, mensaje, BUFFERSIZE,
				0, (struct sockaddr *)&clientaddr_in, addrlen) == -1) {
         		perror("serverUDP");
         		printf("%s: sendto error\n", "serverUDP");
         		return;
         	}
		}//RESPUESTA <NUMERO>
		else if(tipo == 2 && estamosJugando == 1){
			char numStr[BUFFERSIZE];
			char mensaje[BUFFERSIZE];
			i = 0;
			
			//printf("\n%d\t", valorAResolver);
			//printf("\n%d\t", numero);
			//Tiene que enviar si es mayor, menor y el numero de intentos restantes
			//Si se acierta se enviará una respuesta de aviso --> Tenemos un struct en el que esta el mensaje y el numero separados

			if(numIntentos == 0){
				strcpy(mensaje, "375 FALLO");
				estamosJugando = 0;
			}else{

				if(numero > valorAResolver){
					//Es MENOR EL NUMERO				
					strcpy(mensaje, "354 MENOR#");
					sprintf(numStr, "%d",numIntentos);
					strcat(mensaje,numStr);
					

				}else if(numero < valorAResolver){
					//EL NUMERO ES MAYOR
					strcpy(mensaje, "354 MAYOR#");
					sprintf(numStr, "%d",numIntentos);
					strcat(mensaje,numStr);
				}else {

					//ACIERTO
					i=1;
					strcpy(mensaje, "350 ACIERTO#");
					sprintf(numStr, "%d",numIntentos);
					strcat(mensaje,numStr);
					estamosJugando = 0;
					}
			}
			numIntentos = numIntentos - 1;
			//Añadir al log		
			if(aniadirAlLog(mensaje, clientaddr_in, hostname, "UDP", 0) == -1){
				perror("No se ha podido añadir la respuesta al fichero");
			}

			//aniadirCRLF(mensaje, BUFFERSIZE);
			strcat(mensaje, "\r\n");
			if ( nc = sendto (s, mensaje, BUFFERSIZE,
				0, (struct sockaddr *)&clientaddr_in, addrlen) == -1) {
         		perror("serverUDP");
         		printf("%s: sendto error\n", "serverUDP");
         		return;
         	}
			

		}
		//Cliente "+"
		else if(tipo == 3 && (estamosJugando == 0 || numIntentos == 0) && mensajeHola == 1){	//Solo se mete si numero de errores = 0 ooooo hemos terminado){
			char mensaje[BUFFERSIZE];
			valorAResolver = calcularNumeroRandom();		//Numero random entre 2 valores, en este caso, 0 y 100
			numIntentos = 5;
			estamosJugando = 1;
			char numStr[10];

			strcpy(mensaje, "250 Adivina el valor entre 0 y 100#");
			sprintf(numStr, "%d",numIntentos);
			strcat(mensaje, numStr);

			//Añadir al log		
			if(aniadirAlLog(mensaje, clientaddr_in, hostname, "UDP", 0) == -1){
				perror("No se ha podido añadir la respuesta al fichero");
			}
					
			//aniadirCRLF(mensaje, BUFFERSIZE);
			strcat(mensaje, "\r\n");
			if ( nc = sendto (s, mensaje, BUFFERSIZE,
				0, (struct sockaddr *)&clientaddr_in, addrlen) == -1) {
         		perror("serverUDP");
         		printf("%s: sendto error\n", "serverUDP");
         		return;
         	}
			
			
		}
		//Cliente "ADIOS"
		else if(tipo == 4){
			//char mensaje[BUFFERSIZE];;
			//strcpy(mensaje, "221 Cerrando el Servicio");

			//Añadir al log		
			if(aniadirAlLog("221 Cerrando el Servicio", clientaddr_in, hostname, "UDP", 0) == -1){
				perror("No se ha podido añadir la respuesta al fichero");
			}
			
			//aniadirCRLF(mensaje, TAM_BUFFER);
			//strcat(mensaje, "\r\n");
			if ( nc = sendto (s, "221 Cerrando el Servicio\r\n", BUFFERSIZE,
				0, (struct sockaddr *)&clientaddr_in, addrlen) == -1) {
         		perror("serverUDP");
         		printf("%s: sendto error\n", "serverUDP");
         		return;
         	}

			finalizacion = 1;
			
		}
		// Cliente ESCRIBE MAL LA RESPUESTA, DEVUELVE ERROR DE SINTAXIS
		else{
			
			//char mensaje[BUFFERSIZE];
			//strcpy(mensaje, "500 Error de sintaxis\r\n");

			//Añadir al log		
			if(aniadirAlLog("500 Error de sintaxis", clientaddr_in, hostname, "UDP", 0) == -1){
				perror("No se ha podido añadir la respuesta al fichero");
			}

			//aniadirCRLF(mensaje, BUFFERSIZE);
			//strcat(mensaje, "\r\n");
			if ( nc = sendto (s,  "500 Error de sintaxis\r\n", BUFFERSIZE,
					0, (struct sockaddr *)&clientaddr_in, addrlen) == -1) {
         		perror("serverUDP");
         		printf("%s: sendto error\n", "serverUDP");
         		return;
         	}
		}


		if(finalizacion == 1){
			break;
		}
	}  
	printf("Completed %s port %u, at %s\n",
		hostname, ntohs(clientaddr_in.sin_port),(char *) ctime(&timevar));
 }
