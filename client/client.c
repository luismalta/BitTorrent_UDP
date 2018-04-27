#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

int main(int argc, char const *argv[]){
	int porta, connector;
	ssize_t ler_bytes, escrever_bytes;
	int client_socket;
	struct sockaddr_in serv_addr;
	char str[4096];

	if(argc < 3){
		printf("Uso correto: endereco IP - porta\n");
		exit(1);
	}

	client_socket = socket(AF_INET, SOCK_STREAM, 0);

	if(client_socket <= 0){
		printf("Erro no socket: %s\n", strerror(errno));
		exit(1);
	}

	bzero(&serv_addr, sizeof(serv_addr));
	porta = atoi(argv[2]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(porta);

	connector = connect(client_socket, (const struct sockaddr*) &serv_addr, sizeof(serv_addr));
	if(connector < 0){
		fprintf(stderr, ". Falha na conecao\n");
		exit(1);
	}else{
		printf("Conectado com: %s\n", argv[1]);
	}

	while(1){
		printf("Mensagem: ");
		fgets(str, sizeof(str), stdin);
		escrever_bytes = write(client_socket, str, sizeof(str));
		if(escrever_bytes == 0){
			printf("Erro no write: %s\n",strerror(errno));
			printf("Nada escrito.\n");

		}

		ler_bytes = read(client_socket, str, sizeof(str));
		if(ler_bytes <= 0){
			printf("Erro no read: %s\n", strerror(errno));

		}

	}
	close(client_socket);

	return 0;
}
