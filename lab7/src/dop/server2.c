#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "erproc.h"

#define SADDR struct sockaddr
#define SLEN sizeof(struct sockaddr_in)

int main(int argc, char* argv[]) {

	if (argc != 2) {
		printf("You must use 1 arguments: [server port] \n");
		exit(1);
	}

	int i;
	for (i = 1; i < argc; ++i)
	{
		if (atoi(argv[i]) < 1) {
			printf("Args can't be smaller than 1\n");
			exit(1);
		}
	}

	int SERV_PORT = atoi(argv[1]);
    int SERV_PORT_UDP = SERV_PORT+100;
	int BUFSIZE = 256; // надо ли это?
 
	int tcp_server = Socket(AF_INET, SOCK_STREAM, 0); // Создаем tcp сокет на ipV4
	struct sockaddr_in server_adr_tcp = {0};  // Это структура используется, чтобы задать адресс по протоколу ipV4
	struct sockaddr_in client_adr_tcp = {0};
	server_adr_tcp.sin_family = AF_INET; // Задаем семейстов адресов
	// Порт на котором будем слушать. 
	// Так как значение на хосте и в сети может иметь разный порядок байт, 
	// используется фукция измения порядка байт от хоста в сети и наоборот
	server_adr_tcp.sin_port = htons(SERV_PORT);

	Bind(tcp_server, (SADDR *) &server_adr_tcp, sizeof server_adr_tcp);
	Listen(tcp_server, 5); // Слушаем на сокете server вплоть до 5 клиентов может ожидать в очереди
	socklen_t adrlen = sizeof client_adr_tcp;
    int fd_tcp;
 while (1) {
	int fd_tcp = Accept(tcp_server, (SADDR*)&client_adr_tcp, &adrlen); // передаем сокет, переменную куда будет записан адрес и длину этой переменной. Возвращает файловый дискриптер для общения с клиентом
	printf("connection established \n");
	ssize_t nread;
	char buf[BUFSIZE], buf2[BUFSIZE];

	nread = read(fd_tcp, buf, BUFSIZE); // вернет количество считанных байт 

       if (nread == -1) {
		perror("read failed");
		exit(EXIT_FAILURE);
	}
	else if (nread == 0) {
		printf("END OF FILE occured\n");

	}
	strcpy(buf2, buf);
	write(STDOUT_FILENO, buf, nread); // Выводим на консоль солько мы получили байт
    char sendline[BUFSIZE];
    snprintf(sendline, BUFSIZE, "%d", SERV_PORT_UDP);

	write(fd_tcp, sendline, 4); // Отправляет ответ клиенту. На fd передаем buf размером nread;

    break;

  }
  printf("\nStart Udp \n");
	// UDP------------------

	int udp_server = Socket(AF_INET, SOCK_DGRAM, 0); // Создаем udp сокет на ipV4
	struct sockaddr_in server_adr_udp = { 0 };  
	struct sockaddr_in client_adr_udp = { 0 };
	server_adr_udp.sin_family = AF_INET;
	server_adr_udp.sin_port = htons(SERV_PORT_UDP);
    server_adr_udp.sin_addr.s_addr = htonl(INADDR_ANY);
    
	Bind(udp_server, (SADDR*)&server_adr_udp, sizeof server_adr_udp);
	//Listen(udp_server, 5);
	socklen_t adrlen2 = sizeof client_adr_udp;
	//int fd_udp = Accept(udp_server, (SADDR*)&client_adr_udp, &adrlen2);
	printf("connection established \n");
	char sendline[BUFSIZE];
     while (1) {
	for (i = 1; i <= 10; ++i) {
		int j = 0;
		for (j = 0; j < BUFSIZE; ++j)
			sendline[j] = '\0';
		snprintf(sendline, BUFSIZE, "%d", i);

		printf("%s\n", sendline);

		if (sendto(udp_server, sendline, BUFSIZE - 1, 0, (SADDR*)&server_adr_udp, SLEN) == -1) {
			perror("sendto problem");
			exit(1);
		}
        sleep(1);

	}

	write(fd_tcp, "DONE", BUFSIZE);
        break;
     }

	//strcpy(buf, "DONE");
	//write(fd_tcp, buf, nread);

	close(fd_tcp); // Закрываем сокет работы с клиентом
	close(tcp_server); // Закрываем сокет сервера
	//close(fd_udp);
	close(udp_server);
	
	return 0;
}