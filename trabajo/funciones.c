



#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include "funciones.h"








//Funciones


//Eliminamos de string CR-LF
int removeCRLF(char *string){

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
int aniadirAlLog(char *nombre, char *cadena){
	
	FILE *Fich;
	strcat(nombre,".txt");
	if((Fich = fopen(nombre, "a")) == NULL){
		//Fichero corrompido
		return -1;
	}
	
	//Fichero abierto correctamente
	fprintf(Fich,"%s",cadena);

	fclose(Fich);
	return 0;
}

char* aniadirCadena(char* cadena1,char* cadena2){
	strcat(cadena1, cadena2);
	return cadena1;
}


