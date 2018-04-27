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
#include <pthread.h>

#define SizeBuffer 1024

int conta = 0;
int n_arg;
int porta_cliente, porta_servidor, op =0;
FILE *arquivo_entrada;
char nome_arquivo[200];

//==============================================================================
//Leitura do arquivo
//==============================================================================
void ler_arquivo(char nome_arquivo[]){


	arquivo_entrada = fopen(nome_arquivo, "rb");
	if(!arquivo_entrada){
		printf("O arquivo não existe\n");
	}


}

//==============================================================================
//Escrita do arquivo
//==============================================================================
void escrever_arquivo(char nome_arquivo[]){
  char src[10] = "_1";
  strcat(nome_arquivo, src);

	arquivo_entrada = fopen(nome_arquivo, "wb");
	if(!arquivo_entrada){
		printf("O arquivo não existe\n");
	}

}

//==============================================================================
//Função de Cliente
//==============================================================================
void * client_function(){
  printf("Iniciar cliente:\n");
  scanf("%d", &op);
  if(op == 1){
    int connector;
  	ssize_t ler_bytes, escrever_bytes;
  	int client_socket;
  	struct sockaddr_in serv_addr;
  	char str[4096];
    char bufferEntrada[17];
    int bytesRecebidos;


  	if(n_arg < 3){
  		printf("Uso correto: endereco IP - porta\n");
  		exit(1);
  	}

  	client_socket = socket(AF_INET, SOCK_STREAM, 0);

  	if(client_socket <= 0){
  		printf("Erro no socket: %s\n", strerror(errno));
  		exit(1);
  	}

  	bzero(&serv_addr, sizeof(serv_addr));
  	//porta = atoi(argv[2]);
  	serv_addr.sin_family = AF_INET;
  	serv_addr.sin_port = htons(porta_cliente);



  	connector = connect(client_socket, (const struct sockaddr*) &serv_addr, sizeof(serv_addr));
  	if(connector < 0){
  		fprintf(stderr, ". Falha na conecao\n");
  		exit(1);
  	}else{
  		printf("Conectado com: \n");
  	}


  		printf("Requisitar arquivo\n");
      scanf("%s", nome_arquivo);

      int addr_len = sizeof(serv_addr);
  		//fgets(nome_arquivo, sizeof(nome_arquivo), stdin);
  		escrever_bytes = write(client_socket, nome_arquivo, sizeof(nome_arquivo));
  		if(escrever_bytes == 0){
  			printf("Erro no write: %s\n",strerror(errno));
  			printf("Nada escrito.\n");
        close(client_socket);

  		}

  		escrever_arquivo(nome_arquivo);

      bytesRecebidos = recvfrom(client_socket, &bufferEntrada, 17, 0,(struct sockaddr *) &serv_addr, &addr_len);

      fwrite(&bufferEntrada,sizeof(char),bytesRecebidos,arquivo_entrada);


  	close(client_socket);
  }

  return 0;
}
//==============================================================================
//Função de Servidor
//==============================================================================
void * server_function(){
  int server_socket, binder, listener, porta, sock;
	struct sockaddr_in serv_addr, cli_addr;
	ssize_t ler_bytes, escrever_bytes;
	socklen_t clilen;
	char str[4096];
  int bytesEnviados, rc;





	if(n_arg < 3){
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

	//porta = atoi(argv[2]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(porta_servidor);

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



	ler_bytes = read(sock, nome_arquivo, sizeof(nome_arquivo));
	if(ler_bytes <= 0){
		printf("Erro no read: %s\n", strerror(errno));
	}

  ler_arquivo(nome_arquivo);
  printf("Saiu ler arquivo\n");

  fseek(arquivo_entrada,0,SEEK_END);
  long long tamanhoArquivo = ftell(arquivo_entrada);
  printf("%lld\n", tamanhoArquivo);
  fseek(arquivo_entrada,0,SEEK_SET);

  printf("Pegou tamnho\n");

  char bufferEnvio[tamanhoArquivo];
  memset(bufferEnvio,0x0,tamanhoArquivo);

  fread(&bufferEnvio, tamanhoArquivo, 1, arquivo_entrada);

  bytesEnviados = sendto(sock, bufferEnvio, tamanhoArquivo, 0,(struct sockaddr *) &cli_addr, sizeof(cli_addr));

  printf("Enviou bytes\n");
  //verifica se conseguiu enviar
  if(bytesEnviados<0) {
    printf("ERROR: 01\n");
    printf("Cannot send data\n");
    close(sock);
    exit(1);
  }

  if(rc<0) {
    printf("Cannot send data\n");
    close(sock);
    exit(1);
  }

	printf("Enviou\n");

	printf("Mensagem: ");
	fgets(str, sizeof(str), stdin);
	escrever_bytes = write(sock, str, sizeof(str));
	if(escrever_bytes == 0){
		printf("Erro no write: %s\n",strerror(errno));
		printf("Nada escrito.\n");

	}



	close(sock);
	close(server_socket);
  return 0;
}
//==============================================================================
int main(int argc, char const *argv[]) {
  pthread_t server, client;

  n_arg = argc;
  porta_cliente = atoi(argv[2]);
  porta_servidor = atoi(argv[3]);
  //ip = argv[1];

  pthread_create(&server, NULL, server_function,NULL);
  pthread_create(&client, NULL, client_function,NULL);

  pthread_join(client, NULL);
  pthread_join(server, NULL);

  return 0;
}
