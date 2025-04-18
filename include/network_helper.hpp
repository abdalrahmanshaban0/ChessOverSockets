#pragma once
#include <arpa/inet.h>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

class Networking {
private:
  int sktFD;
  sockaddr_in addr;
  void listen();
  void bind();
  void connect();
  int accept();

public:
  Networking(in_addr_t IP);
  int playerInvite();
  int playerSearch();
};
