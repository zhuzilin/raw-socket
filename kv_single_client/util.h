#ifndef RAW_SOCKET_KV_UTIL_H_
#define RAW_SOCKET_KV_UTIL_H_

#define READ(ptr, num, T) \
	  num = *((T *) ptr); \
	  ptr += sizeof(T)

#define WRITE(ptr, num, T) \
    *((T*) ptr) = num; \
    ptr += sizeof(T)

std::string read_str(char **raw) {
  size_t str_len;
  READ(*raw, str_len, size_t);
  std::string str(*raw, str_len);
  // std::cout << "str: " << str << std::endl;
  *raw += str_len;
  return str;
}

void write_str(char **raw, const std::string& str) {
  WRITE(*raw, str.size(), size_t);
  // str
  for (size_t i = 0; i < str.size(); i++) {
    (*raw)[i] = str[i];
  }
  *raw += str.size();
}

#define BUFFER_SIZE 10

int send_message(int fd, const std::string& content) {
  char buffer[BUFFER_SIZE];
  int i = 0, n;
  std::string msg(sizeof(size_t) + content.size(), ' ');
  char *raw = &msg[0];
  write_str(&raw, content);
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

int receive_message(int fd, std::string& msg) {
  char buffer[BUFFER_SIZE];
  int i = 0, n;

  n = read(fd, buffer, sizeof(buffer));
  if (n < 0) {
    printf("failed to read\n");
    return -1;
  } else if (n == 0) {
    printf("EOF\n");
    return -1;
  }
  size_t remain_msg_size;
  char *raw = buffer;
  READ(raw, remain_msg_size, size_t);
  msg = std::string(raw, n - sizeof(size_t));
  remain_msg_size -= n - sizeof(size_t);
  while (remain_msg_size) {
    n = read(fd, buffer, sizeof(buffer));
    if (n < 0) {
      printf("failed to read\n");
      return -1;
    } else if (n == 0) {
      printf("EOF\n");
      return -1;
    }
    std::string seg_msg = std::string(buffer, n);
    msg += seg_msg;
    remain_msg_size -= n;
  }
  return 0;
}

#endif