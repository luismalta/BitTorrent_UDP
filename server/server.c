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
int main(int argc, char const *argv[]){
	int server_socket, binder, listener, porta, sock;
	struct sockaddr_in serv_addr, cli_addr;
	ssize_t ler_bytes, escrever_bytes;
	socklen_t clilen;
	ssize_t resposta;
	int bytesRecebidos;
	char str[4096];
	char musica[100] = "sick_boy.mp3";
	char bufferRastreador[100];

while(1){
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	if(server_socket == 0){
		printf("Erro na abertura do socket: %s\n", strerror(errno));
		exit(1);
	}

	if(server_socket < 0){
		printf("Erro na abertura do socket: %s\n", strerror(errno));
		exit(1);
	}
	else if(server_socket){
		do{
			printf("Aguardando cliente...\n");
		}while(!accept);
	}

	bzero(&serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(3030);

	binder = bind(server_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if(binder < 0){
		printf("Erro no Bind: %s\n", strerror(errno));
		exit(1);
	}

	listener = listen(server_socket, 20);

	if(listener < 0){
		printf("Erro no Listen: %s\n", strerror(errno));
		exit(1);
	}

	clilen = sizeof(cli_addr);

	sock = accept(server_socket, (struct sockaddr*) &cli_addr, &clilen);

	if(sock <= 0){
		printf("Erro no accept: %s\n", strerror(errno));
	}else{
		printf("Conexao recebida de %s\n", inet_ntoa(cli_addr.sin_addr));
	}



	// ler_bytes = read(sock, str, sizeof(str));
	// if(ler_bytes <= 0){
	// 	printf("Erro no read: %s\n", strerror(errno));
	//
	// }
	int addr_len = sizeof(serv_addr);
	bytesRecebidos = read(sock, bufferRastreador, sizeof(bufferRastreador));

	if(!strcmp(bufferRastreador,musica)){
		printf("%s\n\n", bufferRastreador);
		printf("%s\n", musica);
		resposta = sendto(sock, "2021", 4, 0,(struct sockaddr *) &serv_addr, sizeof(serv_addr));

	} else {
		printf("Arquivo inexistente\n");
		resposta = sendto(sock, "0000", 4, 0,(struct sockaddr *) &serv_addr, sizeof(serv_addr));
	}
	close(sock);
	close(server_socket);
}

	return 0;
}
