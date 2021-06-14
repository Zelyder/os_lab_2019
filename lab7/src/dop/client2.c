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
	if (argc != 3) {
		printf("You must use 2 arguments: [server IP address] [port] \n");
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

	int BUFSIZE = 256;
	int SERV_PORT = atoi(argv[2]);
	char buf[BUFSIZE];

	int fd_tcp = Socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_adr_tcp = {0};
	server_adr_tcp.sin_family = AF_INET; // Задаем семейстов адресов
	// Порт на котором будем слушать. 
	// Так как значение на хосте и в сети может иметь разный порядок байт, 
	// используется фукция измения порядка байт от хоста в сети и наоборот
	server_adr_tcp.sin_port = htons(SERV_PORT);
	Inet_pton(AF_INET, argv[1], &server_adr_tcp.sin_addr);
	Connect(fd_tcp, (SADDR*)&server_adr_tcp, sizeof server_adr_tcp); // Подключаемся через сокет fd, указываем адрес сервера

	write(fd_tcp, "GET", 3);
	ssize_t nread;
	nread = read(fd_tcp, buf, BUFSIZE);
	if (nread == -1) {
		perror("read failed");
		exit(EXIT_FAILURE);
	}
	else if (nread == 0) {
		printf("END OF FILE occured\n");
	}
	printf("Ans is %s\n", buf);
    int SERV_PORT_UDP = atoi(buf);
	write(STDOUT_FILENO, buf, nread);
    close(fd_tcp);

	// UDP----------------
    printf("\nStart UDP \n");
    struct sockaddr_in server_adr_udp = { 0 };
    Inet_pton(AF_INET, argv[1], &server_adr_udp.sin_addr);
    printf("\nStart Inet_pton \n");

	int fd_udp = Socket(AF_INET, SOCK_DGRAM, 0);
    printf("\nStart Socket \n");
	char mesg[BUFSIZE], sendline[BUFSIZE];
	int n;
	
	server_adr_udp.sin_family = AF_INET; // Задаем семейстов адресов
	// Порт на котором будем слушать. 
	// Так как значение на хосте и в сети может иметь разный порядок байт, 
	// используется фукция измения порядка байт от хоста в сети и наоборот
	server_adr_udp.sin_port = htons(SERV_PORT_UDP);

    printf("\n finish setup addr %d\n", SERV_PORT_UDP);
    memset(mesg, 0, sizeof(mesg));

     while ((n = read(fd_udp, sendline, BUFSIZE)) > 0) {
    if (sendto(fd_udp, sendline, n, 0, (SADDR *)&server_adr_udp, SLEN) == -1) {
      perror("sendto problem");
      exit(1);
    }

    /* Get echo */
    /* Do not remember sender (NULL, NULL) - no need */
    memset(mesg, 0, sizeof(mesg));
    if (recvfrom(fd_udp, mesg, BUFSIZE, 0, NULL, NULL) == -1) {
      perror("recvfrom problem");
      exit(1);
    }

    printf("REPLY FROM SERVER= %s\n", mesg);
  }

  /*
    printf("\n fun\n");
    
    if (recvfrom(fd_udp, mesg, BUFSIZE, 0, (SADDR*)&server_adr_udp, SLEN) == -1) {
      perror("recvfrom problem");
      exit(1);
    }
     printf("REPLY FROM SERVER= %s\n", mesg);
  

        printf("enter in while");
        n = recvfrom(fd_udp, mesg, BUFSIZE, 0, (SADDR*)&server_adr_udp, &len);
        printf("\recvfrom = %s\n", mesg);
		if (n < 0) {
			perror("recvfrom");
			exit(1);
		}
        printf("pass if ");

		printf("\n%s\n", mesg);

		if (strcmp(mesg, "DONE") == 0)
		{
			printf("client is DONE");
			break;
		} else {
            printf("client not DONE. Ans = %s", mesg);
        }
*/
	
	close(fd_udp);

	return 0;
}