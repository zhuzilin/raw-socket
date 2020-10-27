/*
 * An simple kv. server part.
 * This code can only support one client.
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
#include <unordered_map>

#include "util.h"

int main(int argc, char *argv[]) {
  int listenfd, connfd, n, portno;
  socklen_t client_len;
  struct sockaddr_in server_addr, client_addr;

  std::unordered_map<std::string, std::string> db;

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

  connfd = accept(listenfd, (struct sockaddr *)&client_addr, &client_len);
  if (connfd < 0) {
    printf("failed to accept\n.");
    return -1;
  }

  while (true) {
    std::string request;
    if (receive_message(connfd, request) < 0) {
      printf("receive failed");
      return -1;
    }
    if (request.size() == 0) {
      continue;
    }
    char *raw = &request[0];
    switch (raw[0]) {
      case 'p': {
        std::cout << "put ";
        raw++;
        std::string key = read_str(&raw);
        std::string value = read_str(&raw);
        std::cout << key << " " << value << std::endl;
        db[key] = value;
        if (send_message(connfd, "SUCCESS") < 0) {
          printf("send failed");
          return -1;
        }
        break;
      }
      case 'g': {
        std::cout << "get ";
        raw++;
        std::string key = read_str(&raw);
        std::string value = db[key];
        std::cout << key << " " << value << std::endl;
        std::string reply(sizeof(size_t) + value.size(), ' ');
        char *raw_reply = &reply[0];
        write_str(&raw_reply, value);
        if (send_message(connfd, reply) < 0) {
          printf("send failed");
          return -1;
        }
        break;
      }
    }
  }
  close(connfd);
  close(listenfd);
}