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

#define SADDR struct sockaddr
#define SLEN sizeof(struct sockaddr_in)

int main(int argc, char *argv[]) {

  if (argc != 2) {
    printf("You must use 1 arguments: [server port] \n");
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

  const size_t kSize = sizeof(struct sockaddr_in);

  int lfd, cfd;
  int nread;
  int SERV_PORT = atoi(argv[1]);
  int BUFSIZE = 256;
  char buf[BUFSIZE];
  struct sockaddr_in servaddr;
  struct sockaddr_in cliaddr;
// TCP---------------------------------------------------------------- 
  if ((lfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }

  memset(&servaddr, 0, kSize);
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERV_PORT);

  if (bind(lfd, (SADDR *)&servaddr, kSize) < 0) {
    perror("bind");
    exit(1);
  }

  if (listen(lfd, 5) < 0) {
    perror("listen");
    exit(1);
  }

    
  int num = -1;

  while (1) {
    unsigned int clilen = kSize;

    if ((cfd = accept(lfd, (SADDR *)&cliaddr, &clilen)) < 0) {
      perror("accept");
      exit(1);
    }
    printf("connection established\n");

    while ((nread = read(cfd, buf, BUFSIZE)) > 0) {
      write(1, &buf, nread);
      break;
    }

    if (nread == -1) {
      perror("read");
      exit(1);
    }
    close(cfd);

    num = atoi(buf);
    printf("Gotted num: %d\n", num);
    break;
  }


//UDP ----------------------------------------------------------
    int cntTotal = 0;
  char mesg[BUFSIZE], ipadr[16];;
  int sockfd, n;
  struct sockaddr_in servaddrUdp;
  struct sockaddr_in cliaddrUdp;
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket problem");
    exit(1);
  }
  memset(&servaddrUdp, 0, SLEN);
  servaddrUdp.sin_family = AF_INET;
  servaddrUdp.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddrUdp.sin_port = htons(SERV_PORT);
  
  if (bind(sockfd, (SADDR *)&servaddrUdp, SLEN) < 0) {
    perror("bind problem");
    exit(1);
  }
  printf("SERVER starts...\n");

  while (1) {
    unsigned int len = SLEN;
    int j = 0;
    memset(&mesg, 0, SLEN);

    if ((n = recvfrom(sockfd, mesg, BUFSIZE, 0, (SADDR *)&cliaddrUdp, &len)) < 0) {
      perror("recvfrom failed");
      exit(1);
    }
    cntTotal++;
    sleep(1);
    printf("\n%s -> %d\n", mesg, cntTotal);

    if(strcmp(mesg, "stop")==0)
    {
        printf("Finish \n");
        break;
    }
     
  }
 
//end udp-------------------------------------------------------------------



}