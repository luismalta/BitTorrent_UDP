Grupo:
Bianca da Rocha Bartolomei - 2016001620
Luís Otávio Malta - 2016013442

Como executar:
  Servidor:
    Compilar: gcc server.c -o server -lpthread
    Executar: ./server 127.0.0.1 <nome_arquivo>

  Cliente 1:
    Compilar: gcc torrent_client.c -o client -lpthread
    Executar: ./client 127.0.0.1 2020

  Cliente 2:
    Compilar: gcc torrent_client.c -o client -lpthread
    Executar: ./client 127.0.0.1 2021

Considerações sobre o trabalho:

    Para realização deste trabalho, foi tomado como base os códigos passados em aula e o material disponibilizado no SIGAA.

    Para o funcionamento, deve ser executado dois clientes torrent e um servidor rastreador. Para tanto, foi usado thread a fim de que qualquer cliente aberto pudesse fazer uma requisição ou atendê-la.

    A fim didático, foi decidido colocar o nome do arquivo que se deseja requerir no parâmetro do servidor rastreador.

    Como proposto, foi implementado o modelo UDP, com checksum, numero de sequência, temporizador e pacote de reconhecimento.

Link GitHub: https://github.com/luismalta/BitTorrent_UDP
