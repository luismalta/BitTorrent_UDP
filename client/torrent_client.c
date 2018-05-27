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
#define TEMPORIZADOR 1

int conta = 0;
int n_arg;
int porta_cliente, porta_servidor, op =0;
FILE *arquivo_entrada;
char nome_arquivo[200];
char bufferResposta[2];
pthread_t threadTimer, threadResposta;
int sock;
struct sockaddr_in serv_addr, cli_addr;
int limiteTempo, timer, recebeu;


int checkcheck(char buffer[], int tamanho){

  int i;
  char check=0;
  for(i=0 ; i < tamanho -2 ; i++){

    check += buffer[i];
    check &= 127;
  }
  printf("Verificando Checksum...\n");
  printf("Checksum recebido: 0x%x\n",buffer[tamanho-1]);
  printf("Checksum calculado: 0x%x\n",check);

  if(buffer[tamanho-1] == (check)){
      printf("Pacote não violado!\n");
      return 1;
  }

  else{
      printf("Pacote violado!\n");
      return 0;
  }


}

char doChecksum(char data[], int tamanho){

  int i;
  char checksum = 0;
  for(i = 0 ; i<tamanho; i++){

    checksum += data[i];
    checksum &= 127;
  }
  return checksum;
}


//função do temporizador, será executada em paralelo
void *timerFun(){
    sleep(TEMPORIZADOR);
    limiteTempo = 1;
    recebeu = 0;
    timer = 0;
}

// função para cnacelar um thread
int pthread_cancel(pthread_t thread);



// função que espera epla resposta do servidor, será executada em paralelo
void *respostaFunc(){
  ssize_t resposta;
  int addr_len = sizeof(serv_addr);
  resposta = recvfrom(sock, &bufferResposta, 2, 0,(struct sockaddr *) &serv_addr, &addr_len);
  recebeu = 1;
  timer = 0;
}

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
    ssize_t resposta;
  	int client_socket;
  	struct sockaddr_in serv_addr;
  	char str[4096];
    char bufferEntrada[SizeBuffer + 3], bufferResposta[2], bufferRastreador[4];
    int bytesRecebidos;
    char numeroPacote = 0;


  	if(n_arg < 2){
  		printf("Uso correto: endereco IP - porta\n");
  		exit(1);
  	}

  	client_socket = socket(AF_INET, SOCK_STREAM, 0);

  	if(client_socket <= 0){
  		printf("Erro no socket: %s\n", strerror(errno));
  		exit(1);
  	}

  	bzero(&serv_addr, sizeof(serv_addr));

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
      //sento nome arquivo rastreador
      resposta = sendto(client_socket, nome_arquivo, sizeof(nome_arquivo), 0,(struct sockaddr *) &serv_addr, sizeof(serv_addr));
      //recfrom porta do rastreador
      int addr_len = sizeof(serv_addr);
      bytesRecebidos = recvfrom(client_socket, &bufferRastreador, 4, 0,(struct sockaddr *) &serv_addr, &addr_len);
      //fecha socket
      close(client_socket);

      if(!strcmp(bufferRastreador,"0000")){
        printf("Arquivo inexistente\n");
        return 0;
      }

      //abre socket com o outro cliente
      client_socket = socket(AF_INET, SOCK_STREAM, 0);

    	if(client_socket <= 0){
    		printf("Erro no socket: %s\n", strerror(errno));
    		exit(1);
    	}

    	bzero(&serv_addr, sizeof(serv_addr));

    	serv_addr.sin_family = AF_INET;
    	serv_addr.sin_port = htons(atoi(bufferRastreador));



    	connector = connect(client_socket, (const struct sockaddr*) &serv_addr, sizeof(serv_addr));
    	if(connector < 0){
    		fprintf(stderr, ". Falha na conecao\n");
    		exit(1);
    	}else{
    		printf("Conectado com: \n");
    	}


       addr_len = sizeof(serv_addr);

			escrever_bytes = write(client_socket, nome_arquivo, sizeof(nome_arquivo));
			if(escrever_bytes == 0){
				printf("Erro em manda o nome do arquivo\n");
				close(client_socket);
			}

			escrever_arquivo(nome_arquivo);
			while(1){
				printf("Entrou no while cliente\n");
				bytesRecebidos = recvfrom(client_socket, &bufferEntrada, SizeBuffer+3, 0,(struct sockaddr *) &serv_addr, &addr_len);
				fwrite(&bufferEntrada,sizeof(char),bytesRecebidos-3,arquivo_entrada);

        printf("numeroPacote: %d   Buffer: %d\n", numeroPacote, bufferEntrada[bytesRecebidos]);
        if(numeroPacote > bufferEntrada[bytesRecebidos-1]){
          printf("PACOTE DUPLICADO, foi descartado\n");
          printf("ACK ENVIADO!!!\n");
          bufferResposta[0] = 1;
          bufferResposta[1] = numeroPacote;
          resposta = sendto(client_socket, bufferResposta, 2, 0,(struct sockaddr *) &serv_addr, sizeof(serv_addr));
          continue;
        }


          if(!checkcheck(bufferEntrada,bytesRecebidos-1)){
            printf("NAK N = %d ENVIADO!!!\n",numeroPacote);

            bufferResposta[0] = 2;
            bufferResposta[1] = numeroPacote;
            resposta = sendto(client_socket, bufferResposta, 2, 0,(struct sockaddr *) &serv_addr, sizeof(serv_addr));
  					continue;
  				} else {
            printf("ACK N = %d ENVIADO!!!\n",numeroPacote);
            bufferResposta[0] = 1;
            bufferResposta[1] = numeroPacote;
            resposta = sendto(client_socket, bufferResposta, 2, 0,(struct sockaddr *) &serv_addr, sizeof(serv_addr));
          }


        numeroPacote++;
        numeroPacote %= 128;
				// if(bytesRecebidos < SizeBuffer){
				// 	fclose(arquivo_entrada);
				// 	printf("Saiu do if buffer menor\n");
				// 	break;
				// }

				if(bufferEntrada[bytesRecebidos-3] == '1'){
          printf("%x\n", bufferEntrada[bytesRecebidos-1]);
					fclose(arquivo_entrada);
					printf("Saiu do if buffer igual a 1\n");
					break;
				}


			}




  	close(client_socket);
  }
  printf("Chegou no final clente\n");
  return 0;
}
//==============================================================================
//Função de Servidor
//==============================================================================
void * server_function(){
  int server_socket, binder, listener, porta;
	ssize_t ler_bytes, escrever_bytes;
	socklen_t clilen;
  ssize_t resposta;
	char str[4096];
  int bytesEnviados,bytes_restantes, rc;
	int transferencia_completa = 0, quantidade_bytes_enviados = 0, numero_pacotes_enviados = 0;
	char bufferEnvio[SizeBuffer+3];
	char cont = 0;
  int retornoThread, retornoThread2;




	if(n_arg < 2){
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
  long long tamanho_arquivo = ftell(arquivo_entrada);
  printf("%lld\n", tamanho_arquivo);
  fseek(arquivo_entrada,0,SEEK_SET);

  printf("Pegou tamnho\n");

	while(1){
		printf("Entrou no while servidor\n");
		if(transferencia_completa) break;

		if(quantidade_bytes_enviados + SizeBuffer < tamanho_arquivo){
			printf("Entrou if pacote tamanho normal\n");

			fread(&bufferEnvio, SizeBuffer, 1, arquivo_entrada);
			bufferEnvio[SizeBuffer] = '0';
			bufferEnvio[SizeBuffer+1] = doChecksum(bufferEnvio, SizeBuffer);
			bufferEnvio[SizeBuffer+2] = cont;


			bytesEnviados = sendto(sock, bufferEnvio, SizeBuffer+3, 0,(struct sockaddr *) &cli_addr, sizeof(cli_addr));



			if(bytesEnviados<0) {
		    printf("ERROR: 01\n");
		    printf("Cannot send data\n");
		    close(sock);
		    exit(1);
		  }

      timer = 1;
      recebeu = 0;
      limiteTempo = 0;
      retornoThread = pthread_create( &threadTimer, NULL, timerFun, NULL);

      retornoThread2 = pthread_create( &threadResposta, NULL, respostaFunc, NULL);

      //printf("retorno da thread: %d\n", retornoThread);
      while(timer){
          //espera....

      }

      pthread_cancel(threadResposta);
      pthread_cancel(threadTimer);


      // int addr_len = sizeof(serv_addr);
      // resposta = recvfrom(sock, &bufferResposta, 2, 0,(struct sockaddr *) &serv_addr, &addr_len);


      if(bufferResposta[0] == 1 && recebeu == 1){
        printf("ACK RECEBIDO COM NUMERO DE SEQUENCIA: %d\n", bufferResposta[1]);
        cont++;
        cont %= 128;
      } else {
        if(limiteTempo){
          printf("\nEstorou o temporizador\n");
        } else {
          printf("NAK recebido com numero de sequencia: %d\n",bufferResposta[1] );
        }
        bytesEnviados = sendto(sock, bufferEnvio, SizeBuffer+3, 0,(struct sockaddr *) &cli_addr, sizeof(cli_addr));
      }

			numero_pacotes_enviados++;
			quantidade_bytes_enviados += bytesEnviados;

			memset(bufferEnvio,0x0, SizeBuffer);

		} else {
			printf("Entrou else pacote menor\n");
			bytes_restantes = tamanho_arquivo - quantidade_bytes_enviados;
			memset(bufferEnvio,0x0,SizeBuffer);

			fread(&bufferEnvio, SizeBuffer, 1, arquivo_entrada);
			bufferEnvio[bytes_restantes] = '1';
			bufferEnvio[bytes_restantes+1] = doChecksum(bufferEnvio, bytes_restantes);
			bufferEnvio[bytes_restantes+2] = cont;


			bytesEnviados = sendto(sock, bufferEnvio, bytes_restantes+3, 0,(struct sockaddr *) &cli_addr, sizeof(cli_addr));


			if(bytesEnviados<0) {
		    printf("ERROR: 01\n");
		    printf("Cannot send data\n");
		    close(sock);
		    exit(1);
		  }

      timer = 1;
      recebeu = 0;
      limiteTempo = 0;
      retornoThread = pthread_create( &threadTimer, NULL, timerFun, NULL);

      retornoThread2 = pthread_create( &threadResposta, NULL, respostaFunc, NULL);

      //printf("retorno da thread: %d\n", retornoThread);
      while(timer){
          //espera....

      }

      pthread_cancel(threadResposta);
      pthread_cancel(threadTimer);

      if(bufferResposta[0] == 1 && recebeu == 1){
        printf("ACK RECEBIDO COM NUMERO DE SEQUENCIA: %d\n", bufferResposta[1]);
        cont++;
        cont %= 128;
      } else {
        if(limiteTempo){
          printf("\nEstorou o temporizador\n");
        } else {
          printf("NAK recebido com numero de sequencia: %d\n",bufferResposta[1] );
        }
        bytesEnviados = sendto(sock, bufferEnvio, bytes_restantes+3, 0,(struct sockaddr *) &cli_addr, sizeof(cli_addr));
      }

			numero_pacotes_enviados++;
			quantidade_bytes_enviados += bytesEnviados;

			memset(bufferEnvio,0x0, SizeBuffer);
			transferencia_completa = 1;
		}
	}


  printf("Chegou no final servidor\n");
	close(sock);
	close(server_socket);
  return 0;
}
//==============================================================================
int main(int argc, char const *argv[]) {
  pthread_t server, client;

  n_arg = argc;
  porta_cliente = 3030;
  porta_servidor = atoi(argv[2]);
  //ip = argv[1];

  pthread_create(&server, NULL, server_function,NULL);
  pthread_create(&client, NULL, client_function,NULL);

  pthread_join(client, NULL);
  pthread_join(server, NULL);




  return 0;
}
