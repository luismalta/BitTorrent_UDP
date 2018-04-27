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

int conta = 0;

//==============================================================================
//Função de Cliente
//==============================================================================
void * client_function(){
  int x = 9999;
  while(1){
    printf("fodasee 111\n");
    scanf("%d", &x);
    printf("%d\n",x );
    if(x == 4){
      printf("%d", conta);
    }
  }

  return 0;
}
//==============================================================================
//Função de Servidor
//==============================================================================
void * server_function(){
  while(1){
    //printf("fodasee 222\n");
    conta++;
  }
  return 0;
}
//==============================================================================
int main(int argc, char const *argv[]) {
  pthread_t server, client;

  pthread_create(&server, NULL, server_function,NULL);
  pthread_create(&client, NULL, client_function,NULL);

  pthread_join(client, NULL);
  pthread_join(server, NULL);

  return 0;
}
