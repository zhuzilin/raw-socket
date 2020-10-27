/*
 * An simple raw socket example. client part.
 * The client will send a short message (smaller than buffer size)
 * to server once and receive the reply, then exit.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

int main(int argc, char *argv[]) {
  int fd, n, portno;
  struct sockaddr_in server_addr;
  struct hostent *server;

  if (argc != 3) {
    printf("need ip and port of server.\n");
    return -1;
  }

  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    printf("failed to create socket.\n");
    return -1;
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  portno = atoi(argv[2]);
  server_addr.sin_port = htons(portno);
  server = gethostbyname(argv[1]);
  if (server == NULL) {
    printf("no such host.\n");
    return -1;
  }
  memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

  if (connect(fd, (struct sockaddr *)&server_addr,
      sizeof(server_addr)) < 0) {
    printf("connect failed.\n");
    return -1;
  }

  char buffer[1024];
  memset(buffer, 0, sizeof(buffer));

  printf("Please enter the message: ");
  fgets(buffer, sizeof(buffer)-1, stdin);
  n = write(fd, buffer, strlen(buffer));
  if (n < 0) {
    printf("failed to write\n");
    return -1;
  }
  memset(buffer, 0, sizeof(buffer));
  n = read(fd, buffer, 255);
  if (n < 0) {
    printf("failed to read\n");
    return -1;
  }
  printf("%s\n",buffer);
  close(fd);
}
