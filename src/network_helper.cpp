#include "../include/network_helper.hpp"
#include <cerrno>
#include <cstdlib>
#include <iostream>

using namespace std;

extern char player_name[1024];
extern char guest_name[1024];

Networking::Networking(in_addr_t IP) {
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = IP;

  if ((sktFD = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("Error creating socket");
    exit(1);
  }
}

void Networking::bind() {
  int opt = 1;
  setsockopt(sktFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  if (::bind(sktFD, (sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("Error binding a socket!");
    if (errno == EADDRINUSE)
      cerr << "Wait some seconds and try again\n";
    close(sktFD);
    exit(1);
  }
}

void Networking::listen() {
  if (::listen(sktFD, 5) == -1) {
    perror("Error listening to the socket!");
    close(sktFD);
    exit(1);
  }
}

void Networking::connect() {
  if (::connect(sktFD, (sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("Error connecting to the address!");
    close(sktFD);
    exit(1);
  }
}

int Networking::accept() {
  int clientFD = ::accept(sktFD, nullptr, nullptr);
  if (clientFD < 0) {
    perror("Error accepting connection");
    close(sktFD);
    exit(1);
  }
  return clientFD;
}

int Networking::playerInvite() {
  connect();

  if (!send_all(sktFD, player_name, sizeof(player_name))) {
    perror("error sending player name");
    close(sktFD);
    return -1;
  }

  char ok = 'n';
  if (!recv_all(sktFD, &ok, sizeof(ok)) || ok == 'n') {
    close(sktFD);
    return -1;
  }

  char guestName[1024] = {0};
  if (!recv_all(sktFD, guestName, sizeof(guestName))) {
    perror("Error receiving guest name");
    close(sktFD);
    return -1;
  }
  strncpy(guest_name, guestName, sizeof(guest_name) - 1);
  guest_name[sizeof(guest_name) - 1] = '\0';
  return sktFD;
}

int Networking::playerSearch() {
  bind();
  listen();

  char rp = 'n';
  int clientFD = -1;
  char guestName[1024];

  do {
    cout << "Searching for players..." << endl;
    clientFD = accept();

    memset(guestName, 0, sizeof(guestName));
    if (!recv_all(clientFD, guestName, sizeof(guestName))) {
      perror("Error receiving guest name");
      close(clientFD);
      clientFD = -1;
      continue;
    }

    cout << guestName << " is inviting you to play! (y/n): ";
    cin >> rp;

    if (!send_all(clientFD, &rp, sizeof(rp))) {
      close(clientFD);
      clientFD = -1;
      continue;
    }

    if (rp == 'n') {
      close(clientFD);
      clientFD = -1;
    }
  } while (rp == 'n');

  strncpy(guest_name, guestName, sizeof(guest_name) - 1);
  guest_name[sizeof(guest_name) - 1] = '\0';

  if (!send_all(clientFD, player_name, sizeof(player_name))) {
    perror("error sending player name");
    close(clientFD);
    close(sktFD);
    return -1;
  }

  close(sktFD); // stop listening, keep the connected socket
  return clientFD;
}
