/*
 *			C L I E N T U D P
 *
 *	This is an example program that demonstrates the use of
 *	sockets as an IPC mechanism.  This contains the client,
 *	and is intended to operate in conjunction with the server
 *	program.  Together, these two programs
 *	demonstrate many of the features of sockets, as well as good
 *	conventions for using these features.
 *
 *
 *	This program will request the internet address of a target
 *	host by name from the serving host.  The serving host
 *	will return the requested internet address as a response,
 *	and will return an address of all ones if it does not recognize
 *	the host name.
 *
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

#include "clientudp.h"
#include "funciones.h"

#define ADDRNOTFOUND	0xffffffff	/* value returned for unknown host */
#define RETRIES	5		/* number of times to retry before givin up */
#define BUFFERSIZE	516	/* maximum size of packets to be received */
#define TIMEOUT 6
#define MAXHOST 128
extern int errno;


/*
 *			H A N D L E R
 *
 *	This routine is the signal handler for the alarm signal.
 */
void handler()
{
 printf("Alarma recibida \n");
}

/*
 *			M A I N
 *
 *	This routine is the client which requests service from the remote
 *	"example server".  It will send a message to the remote nameserver
 *	requesting the internet address corresponding to a given hostname.
 *	The server will look up the name, and return its internet address.
 *	The returned address will be written to stdout.
 *
 *	The name of the system to which the requests will be sent is given
 *	as the first parameter to the command.  The second parameter should
 *	be the the name of the target host for which the internet address
 *	is sought.
 */
int clientUDP(char **argv, int argc )
{
	int i, errcode;
	int retry = RETRIES;		/* holds the retry count */
    int s;				/* socket descriptor */
    long timevar;                       /* contains time returned by time() */
    struct sockaddr_in myaddr_in;	/* for local socket address */
    struct sockaddr_in servaddr_in;	/* for server socket address */
    struct in_addr reqaddr;		/* for returned internet address */
    int	addrlen, n_retry;
    struct sigaction vec;
   	char hostname[MAXHOST];
   	struct addrinfo hints, *res;

	
		/* Create the socket. */
	s = socket (AF_INET, SOCK_DGRAM, 0);
	if (s == -1) {
		perror(argv[0]);
		fprintf(stderr, "%s: unable to create socket\n", argv[0]);
		exit(1);
	}
	
    /* clear out address structures */
	memset ((char *)&myaddr_in, 0, sizeof(struct sockaddr_in));
	memset ((char *)&servaddr_in, 0, sizeof(struct sockaddr_in));
	
			/* Bind socket to some local address so that the
		 * server can send the reply back.  A port number
		 * of zero will be used so that the system will
		 * assign any available port number.  An address
		 * of INADDR_ANY will be used so we do not have to
		 * look up the internet address of the local host.
		 */
	myaddr_in.sin_family = AF_INET;
	myaddr_in.sin_port = 0;
	myaddr_in.sin_addr.s_addr = INADDR_ANY;

	if (bind(s, (const struct sockaddr *) &myaddr_in, sizeof(struct sockaddr_in)) == -1) {
		perror(argv[0]);
		fprintf(stderr, "%s: unable to bind socket\n", argv[0]);
		exit(1);
	   }

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
    printf("Connected to %s on port %u at %s", argv[1], ntohs(myaddr_in.sin_port), (char *) ctime(&timevar));

	/* Set up the server address. */
	servaddr_in.sin_family = AF_INET;
		/* Get the host information for the server's hostname that the
		 * user passed in.
		 */
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



   /* Registrar SIGALRM para no quedar bloqueados en los recvfrom */
    vec.sa_handler = (void *) handler;
    vec.sa_flags = 0;
    if ( sigaction(SIGALRM, &vec, (struct sigaction *) 0) == -1) {
            perror(" sigaction(SIGALRM)");
            fprintf(stderr,"%s: unable to register the SIGALRM signal\n", argv[0]);
            exit(1);
        }




	char buffer[BUFFERSIZE];

	//Envio de mensaje "ficticio"
	//Es el envio que sirve para establecer la conexión, es decir, es que provoca que se conecte al ls_UDP
	if (sendto (s, "", BUFFERSIZE,0, (struct sockaddr *)&servaddr_in, addrlen) == -1) {
		perror(argv[0]);
		fprintf(stderr, "%s: unable to send request to \"connect\" \n", argv[0]);
		exit(1);
	}	

	
	FILE *f;
	if(argc == 4){

		//printf("%s", argv[3]);
		f = fopen(argv[3], "r");
		if(f == NULL){
			perror("Error al abrir el archivo");
			return 1;
		}
	}




	int ret;
	int finalizacion = 0;

	while(finalizacion == 0){
		
		
		//Recibe
		int numIntentos = 0;
		while (numIntentos < RETRIES) {
	    	alarm(TIMEOUT);
			//Esperar
			int cc;
        	cc = recvfrom (s, buffer, BUFFERSIZE, 0, (struct sockaddr *)&servaddr_in, &addrlen);
			if(cc == -1){
				//Implica que una señal interrumpió el rcvfrom
				
				if(errno == EINTR){
					numIntentos = numIntentos + 1;
					fprintf(stderr, "Intento %d --> Intentos %d", numIntentos, RETRIES);
				}
				
				else{
					printf("Imposibilidad de llegar el mensaje");
					exit(1);
				}
        	} 
        	else {
            	alarm(0);	
				break;
            	}
			
		}
		//printf("S: %s",buffer);

		
		//Escribimos el mensaje al servidor.
		if(strcmp(buffer, "221 Cerrando el Servicio\r\n")== 0){
			finalizacion = 1;

		}else{

			//printf("C: ");

			//Respuesta
			if(argc == 4){
				
				if(fgets(buffer, BUFFERSIZE-2, f) == NULL){
					return 1;
				}
				
			
			}else if(argc == 3){
				//Respuesta del cliente
				fgets(buffer, BUFFERSIZE-2, stdin);
				int len = strlen(buffer);
				if(len > 0 && buffer[len-1] == '\n'){
					buffer[len-1] = '\0';
				}
			}

			//printf("%s", buffer);


			
			
			
			strcat(buffer,"\r\n");
			/* Send the request to the nameserver. */
        	if (sendto (s, buffer, BUFFERSIZE-2, 0, (struct sockaddr *)&servaddr_in,
					sizeof(struct sockaddr_in)) == -1) {
        		perror(argv[0]);
        		fprintf(stderr, "%s: unable to send request\n", argv[0]);
    			exit(1);
    		}else{
				//printf("envio correcto");
			}
		}

		
	


		

	}	
	printf("All done at %s", (char *)ctime(&timevar));
	
		
}