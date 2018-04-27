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

//==============================================================================
//Função de Cliente
//==============================================================================
void * client_function(void *arg){
  int argv[3];
  argv[2] = 2020;
  int argc = 1;
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
  //porta = atoi(argv[2]);
  porta = argv[2];
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(porta);

  connector = connect(client_socket, (const struct sockaddr*) &serv_addr, sizeof(serv_addr));
  if(connector < 0){
    fprintf(stderr, ". Falha na conecao\n");
    exit(1);
  }else{
    printf("Conectado com\n");
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
//==============================================================================
//Função de Servidor
//==============================================================================
void * server_function(void *arg){
  int argv[3];
  argv[2] = 2020;
  int argc = 1;
  int server_socket, binder, listener, porta, sock;
  struct sockaddr_in serv_addr, cli_addr;
  ssize_t ler_bytes, escrever_bytes;
  socklen_t clilen;
  char str[4096];


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

  //porta = atoi(argv[2]);
  porta = argv[2];
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
//==============================================================================
int main(int argc, char const *argv[]) {
  pthread_t server, client;

  pthread_create(&server, NULL, server_function,(void *)(&argv));
  pthread_create(&client, NULL, client_function,(void *)(&argv));

  pthread_join(client, NULL);
  pthread_join(server, NULL);

  return 0;
}
