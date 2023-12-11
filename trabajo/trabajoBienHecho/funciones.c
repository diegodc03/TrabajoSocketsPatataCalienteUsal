



#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include "funciones.h"








//Funciones

int comprobarMensaje(char *cadena){
	//Hay un mensaje que tiene dos partes, implica que quiero saber las dos
	int i=0;
	
	
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
int aniadirAlLog( char *cadena, struct sockaddr_in clientaddr_in, char *dondeEnvio, char* protocolo, int comprobacion){
	
	FILE *Fich;
	long timevar;
	time_t t = time(&timevar);
	struct tm* ltime = localtime(&t);

	int dia = ltime->tm_mday;
	int mes = ltime->tm_mon + 1;
	int anio = ltime->tm_year + 1900;
	int hora = ltime->tm_hour;
	int minutos = ltime->tm_min;
	int segundos = ltime->tm_sec;


	if((Fich = fopen("peticiones.log", "a")) == NULL){
		//Fichero corrompido
		return -1;
	}
	
	char ipCliente[INET_ADDRSTRLEN]; // Buffer para almacenar la dirección IP del cliente

    // Convierte la dirección IP a una cadena
    if (inet_ntop(AF_INET, &clientaddr_in.sin_addr, ipCliente, INET_ADDRSTRLEN) == NULL) {
        perror("inet_ntop falló");
        return -1; // O manejar el error como prefieras
    }

	
		
	if(comprobacion == 1){
		fprintf(Fich, "[FECHA Y HORA DEL COMIENZO]:  %02d-%02d-%04d | %02d:%02d:%02d || Respuesta enviada al servidor: %s || IP: %s || PROTOCOLO: %s || PUERTO: %u || MENSAJE CLIENTE: %s\n", 
		ltime->tm_mday, ltime->tm_mon+1, ltime->tm_year+1900, hora, minutos, segundos, dondeEnvio, ipCliente, protocolo, ntohs(clientaddr_in.sin_port), cadena);
	}else{
		fprintf(Fich, "[FECHA Y HORA DEL COMIENZO]:  %02d-%02d-%04d | %02d %02d %02d || Respuesta recibida del servidor: %s || IP: %s || PROTOCOLO: %s || PUERTO: %u || MENSAJE SERVIDOR: %s\n", 
		ltime->tm_mday, ltime->tm_mon+1, ltime->tm_year+1900, hora, minutos, segundos, dondeEnvio, ipCliente, protocolo, ntohs(clientaddr_in.sin_port), cadena);
	
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
	//printf("\t%d\n", numeroAleatorio);
	return numeroAleatorio;
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




int recibir(int s, char *buffer, int size, struct sockaddr *servaddr_in, int *addrlen){
	int n_retry;
	n_retry=RETRIES;
	while (n_retry > 0) {
		//RESET(buffer, BUFFERSIZE);		
		/* Set up a timeout so I don't hang in case the packet
		 * gets lost.  After all, UDP does not guarantee
		 * delivery.
		 */
	    alarm(TIMEOUT);
		
		// Wait for the reply to come in. 
		
        if (recvfrom (s, buffer, size, 0,
			(struct sockaddr *)servaddr_in, addrlen) == -1) {
    		if (errno == EINTR) {
    			/* Alarm went off and aborted the receive.
    			 * Need to retry the request if we have
    			 * not already exceeded the retry limit.
    			 */
 			    printf("attempt %d (retries %d).\n", n_retry, RETRIES);
				n_retry--; 
                    } 
            else  {
				printf("Unable to get response from");
				exit(1); 
                }
        } 
        else {
            alarm(0);	
			break;
            }
  
		
    }	
	if (n_retry == 0) {
       	printf("Unable to get response from");
       	
		return -1;
    }
	return 0;

}









