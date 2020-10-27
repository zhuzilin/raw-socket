/*
 * An simple kv. client part.
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

#include "util.h"

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

  while (true) {
    std::string msg, type;
    std::cin >> type;
    if (type == "put") {
      std::string key, value;
      std::cin >> key >> value;
      // type | key_len | key | value_len | value
      msg = std::string(1 + 2 * sizeof(size_t) + key.size() + value.size(), ' ');
      char *raw = &msg[0];
      WRITE(raw, 'p', char);
      write_str(&raw, key);
      write_str(&raw, value);
    } else if (type == "get") {
      std::string key;
      std::cin >> key;
      // type | key_len | key
      msg = std::string(1 + 1 * sizeof(size_t) + key.size(), ' ');
      char *raw = &msg[0];
      WRITE(raw, 'g', char);
      write_str(&raw, key);
    } else {
      continue;
    }
    if (send_message(fd, msg) < 0) {
      printf("send failed");
      return -1;
    }

    std::string reply;
    if (receive_message(fd, reply) < 0) {
      printf("receive failed");
      return -1;
    }

    if (type == "put") {
      std::cout << reply << std::endl;
    } else if (type == "get") {
      char *raw = &reply[0];
      std::string value = read_str(&raw);
      std::cout << "val: " << value << std::endl;
    }
  }
  
  close(fd);
}
