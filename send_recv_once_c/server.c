/*
 * An simple raw socket example. server part.
 * The server will kept on receving short messages (smaller than buffer size)
 * and reply "received!".
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char *argv[]) {
  int listenfd, connfd, n, portno;
  socklen_t client_len;
  struct sockaddr_in server_addr, client_addr;

  if (argc < 2) {
    printf("need port\n");
    return -1;
  }

  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd < 0) {
    printf("failed to create socket.\n");
    return -1;
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  portno = atoi(argv[1]);
  server_addr.sin_port = htons(portno);

  if (bind(listenfd, (struct sockaddr *)&server_addr,
           sizeof(server_addr)) < 0) {
    printf("binding failed.\n");
    return -1;
  }
  // here the number of backlog may be important to certain application.
  if (listen(listenfd, 10) < 0) {
    printf("failed to listen\n");
    return -1;
  }

  char buffer[1024];
  memset(buffer, 0, sizeof(buffer));

  client_len = sizeof(client_addr);
  while (1) {
    connfd = accept(listenfd, (struct sockaddr *)&client_addr, &client_len);
    if (connfd < 0) {
      printf("failed to accept\n.");
      return -1;
    }
    // read from socket
    n = read(connfd, buffer, sizeof(buffer));
    if (n < 0) {
      printf("failed to read\n");
      return -1;
    }
    printf("received: %s\n", buffer);
    // write the same length as the message, no need to add 1 for 
    // the ending EOF.
    n = write(connfd, "received!", 9);
    close(connfd);
  }
  close(listenfd);
}