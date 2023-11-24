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
#include <ctype.h>
#include <time.h>




#define PUERTO 17278
#define ADDRNOTFOUND	0xffffffff	/* return address for unfound host */
#define BUFFERSIZE	1024	/* maximum size of packets to be received */
#define TAM_BUFFER 10
#define MAXHOST 128
#define CR '\r'			//Los declaramos aqui para que sea mas facil el llamdo
#define LF '\n'
#define TC '\0'		//Terminacion de Cadena



#define HOLA 'HOLA'
#define RESPUESTA 'RESPUESTA'
#define SIGNOSUMA '+'
#define ADIOS 'ADIOS'


extern int errno;

/*
 *			M A I N
 *
 *	This routine starts the server.  It forks, leaving the child
 *	to do all the work, so it does not have to be run in the
 *	background.  It sets up the sockets.  It
 *	will loop forever, until killed by a signal.
 *
 */
Msj pasamosMensaje(char *buf);
void serverTCP(int s, struct sockaddr_in peeraddr_in);
void serverUDP(int s, char * buffer, struct sockaddr_in clientaddr_in);
void errout(char *);		/* declare error out routine */

int FIN = 0;             /* Para el cierre ordenado */
void finalizar(){ FIN = 1; }

int main(argc, argv)
int argc;
char *argv[];
{

    int s_TCP, s_UDP;		/* connected socket descriptor */
    int ls_TCP;				/* listen socket descriptor */
    
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
	
	
	/* Create the socket UDP. */
	s_UDP = socket (AF_INET, SOCK_DGRAM, 0);
	if (s_UDP == -1) {
		perror(argv[0]);
		printf("%s: unable to create socket UDP\n", argv[0]);
		exit(1);
	   }
	/* Bind the server's address to the socket. */
	if (bind(s_UDP, (struct sockaddr *) &myaddr_in, sizeof(struct sockaddr_in)) == -1) {
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
		fprintf(stderr, "%s: unable to fork daemon\n", argv[0]);
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
            FD_SET(s_UDP, &readmask);
            /* 
            Seleccionar el descriptor del socket que ha cambiado. Deja una marca en 
            el conjunto de sockets (readmask)
            */ 
    	    if (ls_TCP > s_UDP) s_mayor=ls_TCP;
    		else s_mayor=s_UDP;

            if ( (numfds = select(s_mayor+1, &readmask, (fd_set *)0, (fd_set *)0, NULL)) < 0) {
                if (errno == EINTR) {
                    FIN=1;
		            close (ls_TCP);
		            close (s_UDP);
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
    			if (s_TCP == -1) exit(1);
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
          if (FD_ISSET(s_UDP, &readmask)) {
                /* This call will block until a new
                * request arrives.  Then, it will
                * return the address of the client,
                * and a buffer containing its request.
                * BUFFERSIZE - 1 bytes are read so that
                * room is left at the end of the buffer
                * for a null character.
                */
                cc = recvfrom(s_UDP, buffer, BUFFERSIZE - 1, 0,
                   (struct sockaddr *)&clientaddr_in, &addrlen);
                if ( cc == -1) {
                    perror(argv[0]);
                    printf("%s: recvfrom error\n", argv[0]);
                    exit (1);
                    }
                /* Make sure the message received is
                * null terminated.
                */
                buffer[cc]='\0';
                serverUDP (s_UDP, buffer, clientaddr_in);
                }
          }
		}   /* Fin del bucle infinito de atenci�n a clientes */
        /* Cerramos los sockets UDP y TCP */
        close(ls_TCP);
        close(s_UDP);
    
        printf("\nFin de programa servidor!\n");
        
	default:		/* Parent process comes here. */
		exit(0);
	}

}



typedef struct{
	char mensaje[TAM_BUFFER];
	int numero;
}Msj;


typedef struct{
	int num;
	char cadena[BUFFERSIZE];
}respuestasServidor;







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
	
	//Resolucion Patata Caliente
	int valorAResolver;
	int numIntentos;

	int len, len1, status;
    struct hostent *hp;		/* pointer to host info for remote host */
    long timevar;			/* contains time returned by time() */
	respuestasServidor respServidor;
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


		int i;
	//Finalizar el bucle ya que quiere terminar xd
	int finalizacion = 0;
	//buf es el mensaje que se recibe
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

			if(removeCRLF(buf)){
				fprint(stderr, "Command without CR-LF. Aborted connection\n");
				exit(1);
			}
			//Le hemos quitado el CR-LF

			//Comprubo que es cada cosa, ya que hay respuestas del servidor que tienen un numero y respues
			int tipo;
			Msj msj;
			tipo = comprobacionMensaje(buf);

			//Si retorna 2, es una respuesta, entonces llevara RESPUESA <numero> por lo que lo dividimos
			if(tipo == 2){
				msj = pasamosMensaje(buf);
				strcpy(buf, 'RESPUESTA', sizeof(buf));
			}
			
			//Declaro variables para ver si se puede entrar en los casos o no
			int mensajeHola = 0; //En el momento que entre en Hola ya no podria entrar mas, es el primer mensaje del servidor
			int estamosJugando = 0;


			//Cliente "HOLA"	
			if (tipo == 1 && mensajeHola == 0){

				mensajeHola = 1; //No volvera a entrar
				
				valorAResolver = calcularNumeroRandom();		//Numero random entre 2 valores, en este caso, 0 y 100
				numIntentos = 5;
				char mensajeBase[] = "Adivina el valor entre 0 y 100#";
				char mensaje[BUFFERSIZE];
				char numStr[10];
				
				strcat(mensaje, mensajeBase);
				sprintf(numStr, "%d",numIntentos);
				strcat(mensaje, numStr);
				respServidor = (respuestasServidor) {250,mensaje};
				estamosJugando = 1;
				//Añadimos al log
				
				añadirCRLF(respServidor.cadena, BUFFERSIZE);
				if (send(s, respServidor.cadena, BUFFERSIZE, 0) != BUFFERSIZE) {
					errout(hostname);
				}
			}
			//Cliente RESPUESTA <NUMERO>
			else if(tipo == 2 && estamosJugando == 1){
				char mensaje[BUFFERSIZE];
				char numStr[BUFFERSIZE];
				i = 0;
				numIntentos = numIntentos - 1;
				//Tiene que enviar si es mayor, menor y el numero de intentos restantes
				//Si se acierta se enviará una respuesta de aviso --> Tenemos un struct en el que esta el mensaje y el numero separados
				if(msj.numero > valorAResolver){
					//Es MENOR EL NUMERO
					strcpy(mensaje, "MENOR");
					sprintf(numStr, "%d",numIntentos);
					strcat(mensaje,numStr);

				}else if(msj.numero < valorAResolver){
					//EL NUMERO ES MAYOR
					strcpy(mensaje, "MAYOR");
					sprintf(numStr, "%d",numIntentos);
					strcat(mensaje,numStr);
				}else{
					//ACIERTO
					i=1;
					strcpy(mensaje, "ACIERTO");
					sprintf(numStr, "%d",numIntentos);
					strcat(mensaje,numStr);
				}

				if(i==1){
					//ACIERTO
					respServidor = (respuestasServidor) {350,mensaje};
					estamosJugando = 0;
				}else{
					//NO ACIERTO
					respServidor = (respuestasServidor) {254,mensaje};
				}

				añadirCRLF(respServidor.cadena, BUFFERSIZE);
				if (send(s, respServidor.cadena, BUFFERSIZE, 0) != BUFFERSIZE) {
					errout(hostname);
				}

				//Lo tengo que añadir al log

			}
			//Cliente "+"
			else if(tipo == 3 && (estamosJugando == 0 || numIntentos == 0 && mensajeHola == 1)){	//Solo se mete si numero de errores = 0 ooooo hemos terminado
				//Tiene que enviar <pregunta> # <intentos>

				
					
				valorAResolver = calcularNumeroRandom();		//Numero random entre 2 valores, en este caso, 0 y 100
				numIntentos = 5;
				char mensajeBase[] = "Adivina el valor entre 0 y 100#";
				char mensaje[BUFFERSIZE];
				char numStr[10];
			
				strcat(mensaje, mensajeBase);
				sprintf(numStr, "%d",numIntentos);
				strcat(mensaje, numStr);
				respServidor = (respuestasServidor) {250,mensaje};

				//Añadimos al log
				
					
				

				//Añador el log
				añadirCRLF(respServidor.cadena, BUFFERSIZE);
					if (send(s, respServidor.cadena, BUFFERSIZE, 0) != BUFFERSIZE) {
						errout(hostname);
					}
			}
			//Cliente "ADIOS"
			else if(tipo == 4){
				respServidor = (respuestasServidor) {221, "221 Cerrando el Servicio"};
				
				/*
					if(-1 == addCommandToLog(comResp.message, true)){
					perror("No se ha podido a�adir la respuesta al fichero nntpd.log");
				}
				*/
				
				añadirCRLF(respServidor.cadena, BUFFERSIZE);
				if (send(s, respServidor.cadena, BUFFERSIZE, 0) != BUFFERSIZE) {
					errout(hostname);
				}
				finalizacion = 1;
			}
			// Cliente ESCRIBE MAL LA RESPUESTA, DEVUELVE ERROR DE SINTAXIS
			else{
				respServidor = (respuestasServidor) {500, "500 Error de sintaxis"};

				//Tengo que añadir los comandos a un archivo

				añadorCRLF(respServidor.cadena, BUFFERSIZE);
				if(send(s, respServidor.cadena, BUFFERSIZE, 0) != BUFFERSIZE){
					errout(hostname);
				}
			}


			/* Increment the request count. */
		reqcnt++;
			/* This sleep simulates the processing of the
			 * request that a real server might do.
			 */
		sleep(1);
			/* Send a response back to the client. */
		//if (send(s, buf, TAM_BUFFER, 0) != TAM_BUFFER) errout(hostname);
		if(finalizacion == 1){
			break; 		//Se sale del bucle
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
void serverUDP(int s, char * buffer, struct sockaddr_in clientaddr_in)
{
    struct in_addr reqaddr;	/* for requested host's address */
    struct hostent *hp;		/* pointer to host info for requested host */
    int nc, errcode;

    struct addrinfo hints, *res;

	int addrlen;
    
   	addrlen = sizeof(struct sockaddr_in);

      memset (&hints, 0, sizeof (hints));
      hints.ai_family = AF_INET;
		/* Treat the message as a string containing a hostname. */
	    /* Esta funci�n es la recomendada para la compatibilidad con IPv6 gethostbyname queda obsoleta. */
    errcode = getaddrinfo (buffer, NULL, &hints, &res); 
    if (errcode != 0){
		/* Name was not found.  Return a
		 * special value signifying the error. */
		reqaddr.s_addr = ADDRNOTFOUND;
      }
    else {
		/* Copy address of host into the return buffer. */
		reqaddr = ((struct sockaddr_in *) res->ai_addr)->sin_addr;
	}
     freeaddrinfo(res);

	nc = sendto (s, &reqaddr, sizeof(struct in_addr),
			0, (struct sockaddr *)&clientaddr_in, addrlen);
	if ( nc == -1) {
         perror("serverUDP");
         printf("%s: sendto error\n", "serverUDP");
         return;
         }   
 }



int writeFile(char *buf){
+
}





//Eliminamos de string CR-LF
int removerCRLF(char *string){

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


int añadirCRLF(char *string, int tamanioBuffer){
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


int comprobacionMensaje(char *cadena){

	//Hay un mensaje que tiene dos partes, implica que quiero saber las dos
	int i=0;
	while(cadena[i] != '\0'){

		if(isspace(cadena[i])){
			return 2;
		}
		i++;
	}

	if(strcmp(cadena, "HOLA")){
		return 1;
	}else if(strcmp(cadena, "+")){
		return 3;
	}else if(strcmp(cadena, "ADIOS")){
		return 4;
	}

	return 5;
}


Msj pasamosMensaje(char *cadena){

	Msj msj;
    int i = 0;
    while (cadena[i] != '\0' && !isspace(cadena[i])) {
        msj.mensaje[i] = cadena[i];
        i++;
    }
    msj.mensaje[i] = '\0';


    // Avanzar hasta el primer número
    while (cadena[i] != '\0' && !isdigit(cadena[i])) {
        i++;
    }

	//Processamos el primer numero
	char numBuffer[BUFFERSIZE];
    int j = 0;
    while (isdigit(cadena[i])) {
        numBuffer[j++] = cadena[i++];
    }
    numBuffer[j] = '\0';

    msj.numero = atoi(numBuffer);

    return msj;

}


int calcularNumeroRandom(){
	int numMax = 100;
	int numMin = 0;
	srand((unsigned int)time(NULL));
	 // Calcular el rango del número aleatorio
    int rango = numMax - numMin + 1;

    // Generar el número aleatorio y ajustarlo al rango
    int numeroAleatorio = rand() % rango + numMin;

	return numeroAleatorio;
}


//Como estará en un archivo externo, le pasare, o el nombre del fichero cliente o del fichero servidor
void aniadirAlLog(char *nombre, respuestasServidor resp){
	
	FILE *Fich;
	strcat(nombre,".txt");
	if((Fich = fopen(nombre, "a")) == NULL){
		//Fichero corrompido
		return;
	}
	char* valor[BUFFERSIZE];
	sprintf(valor,"%d ", resp.num);
	strcat(valor, resp.cadena);
	//Fichero abierto correctamente
	fprintf(Fich,valor);


	fclose(Fich);


}























/*

			//Tendre que hacer una funcion en la cual compruebo de se manda, ya que pòr ejemplo respuesta no es solo respuesta, es RESPUESTA <NUMERO>
			switch (tipo)
			{
			case 1: //HOLA
				// Al escribir HOLA lo que se hace es iniciar una nueva pregunta con lo que se envian los max intentos que hay junto con la pregunta
				//Tiene que enviar <pregunta> # <intentos>
				valorAResolver = calcularNumeroRandom();		//Numero random entre 2 valores, en este caso, 0 y 100
				numIntentos = 5;
				char mensajeBase[] = "Adivina el valor entre 0 y 100#";
				char mensaje[BUFFERSIZE];
				char numStr[10];
				
				strcat(mensaje, mensajeBase);
				sprintf(numStr, "%d",numIntentos);
				strcat(mensaje, numStr);
				respServidor = (respuestasServidor) {250,mensaje};

				//Añadimos al log
				
				añadirCRLF(respServidor.cadena, BUFFERSIZE);
				if (send(s, respServidor.cadena, BUFFERSIZE, 0) != BUFFERSIZE) {
					errout(hostname);
				}

				break;
			case 2: //RESPUESTA
				char mensaje[BUFFERSIZE];
				char numStr[BUFFERSIZE];
				i = 0;
				numIntentos = numIntentos - 1;
				//Tiene que enviar si es mayor, menor y el numero de intentos restantes
				//Si se acierta se enviará una respuesta de aviso --> Tenemos un struct en el que esta el mensaje y el numero separados
				if(msj.numero > valorAResolver){
					//Es MENOR EL NUMERO
					strcpy(mensaje, "MENOR");
					sprintf(numStr, "%d",numIntentos);
					strcat(mensaje,numStr);

				}else if(msj.numero < valorAResolver){
					//EL NUMERO ES MAYOR
					strcpy(mensaje, "MAYOR");
					sprintf(numStr, "%d",numIntentos);
					strcat(mensaje,numStr);
				}else{
					//ACIERTO
					i=1;
					strcpy(mensaje, "ACIERTO");
					sprintf(numStr, "%d",numIntentos);
					strcat(mensaje,numStr);
				}

				if(i==1){
					//ACIERTO
					respServidor = (respuestasServidor) {350,mensaje};
				}else{
					//NO ACIERTO
					respServidor = (respuestasServidor) {254,mensaje};
				}

				añadirCRLF(respServidor.cadena, BUFFERSIZE);
				if (send(s, respServidor.cadena, BUFFERSIZE, 0) != BUFFERSIZE) {
					errout(hostname);
				}

				//Lo tengo que añadir al log

				break;

			case 3: //+
				//Tiene que enviar <pregunta> # <intentos>

				if(i == 1 || numIntentos == 0){
					
					valorAResolver = calcularNumeroRandom();		//Numero random entre 2 valores, en este caso, 0 y 100
					numIntentos = 5;
					char mensajeBase[] = "Adivina el valor entre 0 y 100#";
					char mensaje[BUFFERSIZE];
					char numStr[10];
				
					strcat(mensaje, mensajeBase);
					sprintf(numStr, "%d",numIntentos);
					strcat(mensaje, numStr);
					respServidor = (respuestasServidor) {250,mensaje};

					//Añadimos al log
				
					
				}else{
					
					respServidor = (respuestasServidor) {221, "221 Cerrando el Servicio"};

				}

				//Añador el log
				añadirCRLF(respServidor.cadena, BUFFERSIZE);
					if (send(s, respServidor.cadena, BUFFERSIZE, 0) != BUFFERSIZE) {
						errout(hostname);
					}


				break;

			case 4:		//ADIOS 	Se cierra el servicio
				
				
				respServidor = (respuestasServidor) {221, "221 Cerrando el Servicio"};
				
				/*
					if(-1 == addCommandToLog(comResp.message, true)){
					perror("No se ha podido a�adir la respuesta al fichero nntpd.log");
				}
				
				
				añadirCRLF(respServidor.cadena, BUFFERSIZE);
				if (send(s, respServidor.cadena, BUFFERSIZE, 0) != BUFFERSIZE) {
					errout(hostname);
				}
				finalizacion = 1;
				break;

			default:			//Incorrecto	Todo lo que no sea las otras respuestas sera erroneo
				

				respServidor = (respuestasServidor) {500, "500 Error de sintaxis"};

				//Tengo que añadir los comandos a un archivo

				añadorCRLF(respServidor.cadena, BUFFERSIZE);
				if(send(s, respServidor.cadena, BUFFERSIZE, 0) != BUFFERSIZE){
					errout(hostname);
				}

				break;
			}
		}

*/
