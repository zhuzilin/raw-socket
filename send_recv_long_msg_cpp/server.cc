/*
 * An simple raw socket example. server part.
 * Server will receive request in any length and reply with
 * the same message.
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include <iostream>
#include <string>

#define BUFFER_SIZE 4

int send_message(int fd, const std::string& msg) {
  char buffer[BUFFER_SIZE];
  int i = 0, n;
  while (i < msg.size()) {
    if (msg.size() - i < sizeof(buffer)) {
      n = write(fd, msg.substr(i).c_str(), msg.size() - i);
    } else {
      n = write(fd, msg.substr(i, sizeof(buffer)).c_str(), sizeof(buffer));
    }
    if (n < 0) {
      printf("failed to write\n");
      return -1;
    }
    i += n;
  }
  return 0;
}

int receive_message(int fd, std::string& reply_msg) {
  char buffer[BUFFER_SIZE];
  int i = 0, n;
  reply_msg = "";
  while (true) {
    n = read(fd, buffer, sizeof(buffer));
    if (n < 0) {
      printf("failed to read\n");
      return -1;
    }
    std::string seg_msg = std::string(buffer, n);
    reply_msg += seg_msg;
    if (n < sizeof(buffer)) {
      break;
    }
  }
  return 0;
}

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

  client_len = sizeof(client_addr);
  while (1) {
    connfd = accept(listenfd, (struct sockaddr *)&client_addr, &client_len);
    if (connfd < 0) {
      printf("failed to accept\n.");
      return -1;
    }
    std::string request;
    if (receive_message(connfd, request) < 0) {
      printf("receive failed");
      return -1;
    }

    if (send_message(connfd, request) < 0) {
      printf("send failed");
      return -1;
    }

    close(connfd);
  }
  close(listenfd);
}