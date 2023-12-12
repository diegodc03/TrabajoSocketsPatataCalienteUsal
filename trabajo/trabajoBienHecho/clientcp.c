
#include "clientcp.h"



int clienTCP(char **argv, int argc)
{
    int s;				/* connected socket descriptor */
   	struct addrinfo hints, *res;
    long timevar;			/* contains time returned by time() */
    struct sockaddr_in myaddr_in;	/* for local socket address */
    struct sockaddr_in servaddr_in;	/* for server socket address */
	int addrlen, i, j, errcode;
    /* This example uses TAM_BUFFER byte messages. */
	char buf[TAM_BUFFER];
	const char *subcadena = "\r\n";


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

	
	FILE *f;
	if(argc == 4){
		//Introduzco en un array todas las tespuestas
		//printf("%s", argv[3]);
		f = fopen(argv[3], "r");
		if(f == NULL){
			perror("Error al abrir el archivo");
			return 1;
		}
	}

	//Fichero de escritura
	FILE *e;
	if(argc == 4){
		char mensaje[TAM_BUFFER];
		strcat(mensaje, argv[3]);
		strcat(mensaje, "creadoTXT");
		e = fopen(mensaje,"w");
		if(e == NULL){
			perror("Error al abrir el archivo");
			return 1;
		}
	}

	
		

		/* Now, start receiving all of the replys from the server.
		 * This loop will terminate when the recv returns zero,
		 * which is an end-of-file condition.  This will happen
		 * after the server has sent all of its replies, and closed
		 * its end of the connection.
		 */


	int aux;
	char hostName[BUFFERSIZE];

	// Asume que addr ha sido rellenado con la dirección del cliente/servidor
	int result = getnameinfo((struct sockaddr *)&myaddr_in, addrlen, hostName, sizeof(hostName), NULL, 0, 0);

	 int cont;


	while (i = recv(s, buf, TAM_BUFFER, 0)) {
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
		while (i < TAM_BUFFER) {
			j = recv(s, &buf[i], TAM_BUFFER-i, 0);
			if (j == -1) {
                     perror(argv[0]);
			         fprintf(stderr, "%s: error reading result\n", argv[0]);
			         exit(1);
               }
			i += j;
		}

		//Eliminamos CRLF
		aux = eliminarCRLF(buf);
		
/*
		//EMPEZAMOS FUNCIONALIDAD DEL PROGRAMA
		if(strcmp(buf, "220 Servicio Preparado")== 0){
			printf("S: %s\n",buf);
		}
		else if(strcmp(buf,"221 Cerrando el Servicio") == 0){
			printf("S: %s\n",buf);
			break;
		}else{

			//Respuesta del servidor
			//printf("S: %s\n",buf);
			
		}
		//printf("C: ");
*/	
		
		//En vez de leer de pantalla, leo de fichero
		if(argc == 4){
			
			if(fgets(buf, TAM_BUFFER-2, f) == NULL){
				return 1;
			}
			fprintf(e, "%s\n", buf);
			

		}else{
			//Respuesta del cliente manualmente
			fgets(buf, TAM_BUFFER-2, stdin);
			int len = strlen(buf);
			if(len > 0 && buf[len-1] == '\n'){
				buf[len-1] = '\0';
			}
		}

		//printf("%s", buf);

		strcat(buf,"\r\n");
		if(send(s,buf,TAM_BUFFER,0)!=TAM_BUFFER){
			fprintf(stderr,"error");
			exit(1);
		}


	}
	fclose(f);
	fclose(e);
    /* Print message indicating completion of task. */
	time(&timevar);
	printf("All done at %s", (char *)ctime(&timevar));
}

