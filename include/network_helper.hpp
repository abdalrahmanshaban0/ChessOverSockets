#pragma once
#include <arpa/inet.h>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

// Reliable all-or-nothing send/recv helpers.
inline bool send_all(int fd, const void *buf, size_t len) {
  const char *p = static_cast<const char *>(buf);
  while (len > 0) {
    ssize_t k = ::send(fd, p, len, 0);
    if (k <= 0)
      return false;
    p += k;
    len -= static_cast<size_t>(k);
  }
  return true;
}

inline bool recv_all(int fd, void *buf, size_t len) {
  char *p = static_cast<char *>(buf);
  while (len > 0) {
    ssize_t k = ::recv(fd, p, len, 0);
    if (k <= 0)
      return false;
    p += k;
    len -= static_cast<size_t>(k);
  }
  return true;
}

class Networking {
private:
  int sktFD;
  sockaddr_in addr;

  void bind();
  void listen();
  int accept();
  void connect();

public:
  Networking(in_addr_t IP);
  int playerInvite();
  int playerSearch();
};
