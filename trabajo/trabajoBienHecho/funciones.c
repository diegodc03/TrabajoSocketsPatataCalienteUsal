



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
    }
}


int aniadirAlLog(char *nombreFichero, char *cadena){
	
	FILE *Fich;
	strcat(nombreFichero,".txt");
	if((Fich = fopen(nombreFichero, "a")) == NULL){
		//Fichero corrompido
		return -1;
	}
	
	//Fichero abierto correctamente
	fprintf(Fich,"%s",cadena);

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


void dividirCadena(char *cadena, int *numero, char *frase) {
    // Usar strtok para dividir la cadena en espacios
    char *token = strtok(cadena, " ");
    
    // Convertir el primer token en número
    *numero = atoi(token);

    // Usar strtok(NULL, "") para obtener el resto de la cadena como frase
    token = strtok(NULL, "");
    strcpy(frase, token);
}








