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



#define CR '\r'			//Los declaramos aqui para que sea mas facil el llamdo
#define LF '\n'
#define TC '\0'		//Terminacion de Cadena


			
#define PUERTO 17278
#define ADDRNOTFOUND	0xffffffff	/* return address for unfound host */
#define BUFFERSIZE	516	/* maximum size of packets to be received */
#define TAM_BUFFER 10
#define MAXHOST 128

extern int errno;

int comprobarMensaje(char *);
int obtenerNumero(char *);
int aniadirAlLog(char *, int);
int calcularNumeroRandom();
void aniadirCRLF(char *, int );
int eliminarCRLF(char *string);

/*
 *			M A I N
 *
 *	This routine starts the server.  It forks, leaving the child
 *	to do all the work, so it does not have to be run in the
 *	background.  It sets up the sockets.  It
 *	will loop forever, until killed by a signal.
 *
 */
 
void serverTCP(int s, struct sockaddr_in peeraddr_in);
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
        					serverTCP(s_TCP, clientaddr_in);
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
					

    			
				
                	cc = recvfrom(ls_UDP, buffer, BUFFERSIZE, 0,
                   		(struct sockaddr *)&clientaddr_in, &addrlen);
                	if ( cc == -1) {
                    	perror(argv[0]);
                    	printf("%s: recvfrom error\n", argv[0]);
                    	exit (1);
                    	}


					///////////////////////////////////////////
					FILE *file;
					// Abre un archivo para escritura
    				file = fopen("salida.txt", "w");
    				if (file == NULL) {
        			perror("Error al abrir el archivo");
        			return 1;
    				}

    				// Escribe en el archivo
    				fprintf(file, "Estoy despues de leer y deberia recoger, %s.\n",buffer);

    				// Cierra el archivo
    				fclose(file);
					////////////////////////////////////
				


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
				    			
				    		/* Sends a message to the client for him to know the new port for 
							 * the false connection
				    		 */
				    		/*
							if (sendto(s_UDP, "Servicio Preparado\r\n", BUFFERSIZE, 0, (struct sockaddr *)&clientaddr_in, addrlen) == -1) {
								perror(argv[0]);
								fprintf(stderr, "%s: unable to send request to \"connect\" \n", argv[0]);
								exit(1);
							}*/

							//Registers info of the new UDP "false connection"
							//if(-1 == addNewConexionToLog(myaddr_in, clientaddr_in, "UDP")){
							//	perror("No se ha podido a�adir la connection a nntpd.log");
							//}	
								
							//Starts up the server									
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
void serverTCP(int s, struct sockaddr_in clientaddr_in)
{
	int reqcnt = 0;		/* keeps count of number of requests */
	char buf[TAM_BUFFER];		/* This example uses TAM_BUFFER byte messages. */
	char hostname[MAXHOST];		/* remote host's name string */

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
		sleep(1);
			/* Send a response back to the client. */
		if (send(s, buf, TAM_BUFFER, 0) != TAM_BUFFER) errout(hostname);
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


	//Declaraciones para hacer la funcionalidad
	int tipo;
	int estamosJugando = 0;
	int mensajeHola = 0;
	int valorAResolver = 0;	
	int aux;
	int numero;
	int numIntentos;

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


	//Enviamos Primer mensaje al cliente
	if ( nc = sendto (s, "Servicio Preparado\r\n", BUFFERSIZE,
			0, (struct sockaddr *)&clientaddr_in, addrlen) == -1) {
        perror("serverUDP");
        printf("%s: sendto error\n", "serverUDP");
        return;
    }
	
	while(1){
		
		//Recibe la respuesta y despues el servidor hace lo necesario
		cc = recvfrom(s, buf, BUFFERSIZE, 0,
                   		(struct sockaddr *)&clientaddr_in, &addrlen);
    	if ( cc == -1) {
        
        	printf(": recvfrom error\n");
        	exit (1);
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
			printf("numero %d",numero);
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

			/*
			//Añadir al log		
			if(aniadirAlLog(mensaje, 1) == -1){
				perror("No se ha podido añadir la respuesta al fichero");
			}*/

			strcat(mensaje, "\r\n");
			if ( nc = sendto (s, mensaje, BUFFERSIZE,
				0, (struct sockaddr *)&clientaddr_in, addrlen) == -1) {
         		perror("serverUDP");
         		printf("%s: sendto error\n", "serverUDP");
         		return;
         	}
		}







		
	}
	   
 }


int comprobarMensaje(char *cadena){
	//Hay un mensaje que tiene dos partes, implica que quiero saber las dos
	int i=0;
	
	/*
	char token[TAM_BUFFER];
	token = strtok(cadena, "");
	if(strncmp(token, "RESPUESTA") == 0){
		return 2;
	}*/

	if(strcmp(cadena, "HOLA") == 0){
		return 1;
	}else if(strncmp(cadena, "RESPUESTA", 8) == 0){
		return 2;
	}else if(strcmp(cadena, "+") == 0){
		return 3;
	}else if(strcmp(cadena, "ADIOS") == 0){
		return 4;
	}
	return 5;
}


int obtenerNumero(char *cadena){

	int numero = 0;
    char *token;

	if (sscanf(cadena, "RESPUESTA %d", &numero) == 1) {
        return numero;
    }else{
		return -1;
	}

}

//Como estará en un archivo externo, le pasare, o el nombre del fichero cliente o del fichero servidor
int aniadirAlLog( char *cadena, int valor){
	
	FILE *Fich;
	long timevar;
	time_t t = time(&timevar);
	struct tm* ltime = localtime(&t);

	int hora = ltime->tm_hour;
	int minutos = ltime->tm_min;
	int segundos = ltime->tm_sec;


	if((Fich = fopen("peticiones.log", "a")) == NULL){
		//Fichero corrompido
		return -1;
	}
	
	
	//Añadios el mensaje, pero el primer mensaje solo es la hora
	if(valor == 1){
		fprintf(Fich, "HORA: %02d:%02d:%02d | \t", hora, minutos, segundos);
		fprintf(Fich, "RESPUESTA SERVIDOR: %s\n", cadena);
	} else{
		fprintf(Fich, "FECHA Y HORA DEL COMIENZO:  %02d-%02d-%04d | ", ltime->tm_mday, ltime->tm_mon+1, ltime->tm_year+1900);
	}
				

	

	fclose(Fich);
	return 0;
}



int calcularNumeroRandom(){
	int numMax = 10;
	int numMin = 0;
	srand((unsigned int)time(NULL));
	 // Calcular el rango del número aleatorio
    int rango = numMax - numMin + 1;

    // Generar el número aleatorio y ajustarlo al rango
    int numeroAleatorio = rand() % rango + numMin;
	printf("\t%d\n", numeroAleatorio);
	return numeroAleatorio;
}

void aniadirCRLF(char *string, int tamanioBuffer){
	int tamanio;


	tamanio = strlen(string);

	if(tamanio >= tamanioBuffer-2){
		string[tamanioBuffer-2] = CR;
		string[tamanioBuffer-1] = LF;
	}else{
		string[tamanio] = CR;
		string[tamanio + 1] = LF;
	}
    
}



//Eliminamos de string CR-LF
int eliminarCRLF(char *string){

	int i=0;
	//Bucle infinito ya que retornaremos un valor, y luego como se pasa por refrencia la cadena no hay que retornarña
	while(1){
		if(string[i]== CR && string[i+1] == LF){
			string[i] = TC;
			return 0;
		}
		if (i==BUFFERSIZE-2){
			return 1;
		}
	
		i++;
	}




}

