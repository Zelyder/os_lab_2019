#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "erproc.h"

#define SADDR struct sockaddr
#define SIZE sizeof(struct sockaddr_in)
#define SLEN sizeof(struct sockaddr_in)

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("You must input 2 arguments: [server IP address] [port] \n");
    exit(1);
  }

  int i;
  for(i = 1; i < argc; ++i)
  {
    if(atoi(argv[i]) < 1){
      printf("Args can't be smaller than 1\n");
      exit(1);
    }
  }

  int fd;
  int nread;
  int BUFSIZE = 256;
  int SERV_PORT = atoi(argv[2]);
  char buf[BUFSIZE];
  struct sockaddr_in servaddr;
    // TCP-------------------------------------------------------
  fd = Socket(AF_INET, SOCK_STREAM, 0);
  memset(&servaddr, 0, SIZE);
  servaddr.sin_family = AF_INET;

  Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

  servaddr.sin_port = htons(SERV_PORT);

  Connect(fd, (SADDR *)&servaddr, SIZE);

int num = 0;

  write(1, "Input number to send\n", 22);
  while ((nread = read(0, buf, BUFSIZE)) > 0) {
    num = atoi(buf);
    if (write(fd, buf, nread) < 0) {
      perror("write failed");
      exit(1);
    }
    break;
  }

  // UDP-------------------------------------------------------
  int sockfd, n;
  char sendline[BUFSIZE], recvline[BUFSIZE + 1];
  struct sockaddr_in servaddrUDP;

  memset(&servaddrUDP, 0, sizeof(servaddrUDP));
  servaddrUDP.sin_family = AF_INET;
  servaddrUDP.sin_port = htons(SERV_PORT);

  Inet_pton(AF_INET, argv[1], &servaddrUDP.sin_addr);
  sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

  for(i = 1; i <= num; ++i){
      int j = 0;
      for(j = 0; j < BUFSIZE; ++j)
        sendline[j] = '\0';
      snprintf(sendline, BUFSIZE, "%d", i);
      
        printf("%s\n", sendline);

    if (sendto(sockfd, sendline, BUFSIZE-1, 0, (SADDR *)&servaddrUDP, SLEN) == -1) {
      perror("sendto problem");
      exit(1);
    }

  }
  if (sendto(sockfd, "stop", BUFSIZE-1, 0, (SADDR *)&servaddrUDP, SLEN) == -1) {
      perror("sendto problem");
      exit(1);
    }

  close(sockfd);
  close(fd);
  exit(0);
}