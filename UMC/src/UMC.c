/*
 ============================================================================
 Name        : UMC.c
 Author      : JCD
 Version     :
 Copyright   : Your copyright notice
 Description : Proceso UMC - Unidad de control de memoria: Proceso encargado
               de la gestión de la memoria para Elestac.
 ============================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <commons/error.h>


// Puertos en total: 65535
// Puertos registrados: 1024/49151
// Puertos dinámicos/privados: 49152/65535

//#define PUERTO "60201"
#define PUERTO "0201"
#define BACKLOG 100
#define PACKAGESIZE 1024

int main( int argc, char** argv) {

	struct addrinfo hints;
	struct addrinfo *serverInfo;

	typedef struct {
		char PID[50];
		long int memory_size;
	} info_proc_t;


	// Lectura del archivo de configuración:
	typedef struct {

		int PUERTO_RECV_CONEX;
		long int IP_SWAP;
		int PUERTO_SWAP;
		int MARCOS;
		int MARCO_SIZE;
		int MARCO_X_PROC;
		int ENTRADAS_TLB;
		int RETARDO;
	} UMCconfigfile_t;

	FILE *config_file;

	if ((config_file = fopen("/home/utnso/Escritorio/ConfigFile_UMC", "r+")) == NULL){
		perror("\n Error en apertura de archivo de configuración ('fopen()')");
		exit(EXIT_FAILURE);
	}

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	if (getaddrinfo(NULL, PUERTO, &hints, &serverInfo) != 0){
		error_show("\n Ocurrio un problema ('getaddrinfo()')");
		exit(EXIT_FAILURE);
	}

	int listenningSocket;

	if (( listenningSocket = socket(serverInfo->ai_family,
			                        serverInfo->ai_socktype,
							        serverInfo->ai_protocol)) == (-1)){

		error_show("\n Ocurrio un problema al abrir el socket ('socket( )')");
		exit(EXIT_FAILURE);

	}


	 if ((bind(listenningSocket,
	           serverInfo->ai_addr,
			   serverInfo->ai_addrlen)) < 0 ){

		 error_show("\n Ocurrio un problema al asignar nombre a socket ('bind( )')");
		 exit(EXIT_FAILURE);
	 }

	puts("Conexion ok");

	freeaddrinfo(serverInfo);

	listen(listenningSocket, BACKLOG);

    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    int socketCliente = accept(listenningSocket, (struct sockaddr *) &addr, &addrlen);

    char package[PACKAGESIZE];
    int status = 1;

	printf("Cliente conectado. Esperando mensajes:\n");

	while (status != 0){
		status = recv(socketCliente, (void*) package, PACKAGESIZE, 0);
		if (status != 0) printf("%s", package);

	}

	/*
	 * Finaliza el proceso de conexion y el Servidor queda a
	 * la espera de información: solicitudes del núcleo y las
	 * CPU
	 */

	/*
	 * Por cada conexion, la UMC creará dedicado a atenderlo
	 * y el mismo quedará a la espera de solicitudes de operacions
	 */

	/*
	 * Sin memoria -> "CALL" "int* proceso_swap( estructura_proceso_solicitante )"
	 * */



	//close(socketCliente);
	//close(listenningSocket);

	return EXIT_SUCCESS;
}
