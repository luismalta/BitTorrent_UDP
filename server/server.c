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
FILE *arquivo_entrada;


//==============================================================================
//Abertura do arquivo
void abrir_arquivo(){
	char *nome_arquivo;

	nome_arquivo = (char*) calloc(50, sizeof(char));
	printf("Nome do arquivo:\n");
	scanf("%s", nome_arquivo);
	arquivo_entrada = fopen(nome_arquivo, "rb");
	if(!arquivo_entrada){
		printf("O arquivo não existe\n");
	}
}

//==============================================================================
int main(int argc, char const *argv[]){
	int server_socket, binder, listener, porta, sock;
	struct sockaddr_in serv_addr, cli_addr;
	ssize_t ler_bytes, escrever_bytes;
	socklen_t clilen;
	char str[4096];

	abrir_arquivo();


	if(argc < 3){
		printf("Uso correto: endereco IP - porta\n");
		exit(1);
	}

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

	porta = atoi(argv[2]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(porta);

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

while(1){

	ler_bytes = read(sock, str, sizeof(str));
	if(ler_bytes <= 0){
		printf("Erro no read: %s\n", strerror(errno));

	}

	printf("Cliente: %s", str);

	printf("Mensagem: ");
	fgets(str, sizeof(str), stdin);
	escrever_bytes = write(sock, str, sizeof(str));
	if(escrever_bytes == 0){
		printf("Erro no write: %s\n",strerror(errno));
		printf("Nada escrito.\n");

	}
}


	close(sock);
	close(server_socket);

	return 0;
}
