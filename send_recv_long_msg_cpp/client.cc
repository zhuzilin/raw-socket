/*
 * An simple raw socket example. client part.
 * Client will send a message in any length and the reply will
 * be the same message.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

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

  std::string msg;
  std::cout << "message to send:" << std::endl;
  std::cin >> msg;
  if (send_message(fd, msg) < 0) {
    printf("send failed");
    return -1;
  }
  std::string reply;
  if (receive_message(fd, reply) < 0) {
    printf("receive failed");
    return -1;
  }
  std::cout << "receive: " << reply << std::endl;
  close(fd);
}
