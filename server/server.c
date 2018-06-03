#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


//==============================================================================
//Servidor rastreador
int main(int argc, char const *argv[]){
	int server_socket, binder;
	struct sockaddr_in serv_addr, cli_addr;
	ssize_t ler_bytes, escrever_bytes, resposta;
	socklen_t clilen;
	char buffer_rastreador[100];


	while(1){
		//Criação do socket
		server_socket = socket(AF_INET, SOCK_DGRAM, 0);

		if(server_socket <= 0){
			printf("Erro na abertura do socket: %s\n", strerror(errno));
			exit(1);
		}
		else if(server_socket){
			do{
				printf("Aguardando cliente...\n");
			}while(!accept);
		}

		//Conexão com qualquer porta
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		serv_addr.sin_port = htons(3030);

		binder = bind(server_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
		if(binder < 0){
			printf("Erro no Bind: %s\n", strerror(errno));
			exit(1);
		}

		clilen = sizeof(cli_addr);
		ler_bytes = recvfrom(server_socket, &buffer_rastreador, sizeof(buffer_rastreador), 0,(struct sockaddr*) &cli_addr, &clilen);

		//Compara se o nome do arquivo requisitado no buffer_rastreador
		//É igual ao nome da música
		if(!strcmp(buffer_rastreador, argv[2])){
			printf("Enviando porta do cliente localizado\n");
			resposta = sendto(server_socket, "2021", 4, 0,(struct sockaddr *) &cli_addr, sizeof(cli_addr));
		} else {
			printf("Arquivo inexistente\n");
			resposta = sendto(server_socket, "0000", 4, 0,(struct sockaddr *) &cli_addr, sizeof(cli_addr));
		}

		close(server_socket);
}

	return 0;
}
