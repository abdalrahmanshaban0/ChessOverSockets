#include "../include/network_helper.hpp"
#include <iostream>
#include <sys/socket.h>
using namespace std;

extern char player_name[1024];
extern char guest_name[1024];

Networking::Networking(in_addr_t IP) {
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = IP;

  if ((sktFD = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("Error creating socket\n");
    exit(1);
  }
}

void Networking::bind() {
  // bind a socket
  if (::bind(sktFD, (sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("Error binding a socket!\n");
    close(sktFD);
    exit(1);
  }
}

void Networking::listen() {
  // listening to assigned socket
  if (::listen(sktFD, 5) == -1) {
    perror("Error listening to the socket!\n");
    close(sktFD);
    exit(1);
  }
}

void Networking::connect() {
  // sending connection request
  if (::connect(sktFD, (sockaddr *)&addr, sizeof(addr))) {
    perror("Error connecting to the address!\n");
    close(sktFD);
    exit(1);
  }
}

int Networking::accept() {
  // Accept connection
  int clientFD = ::accept(sktFD, nullptr, nullptr);
  if (clientFD < 0) {
    perror("Error accepting connection\n");
    close(sktFD);
    exit(1);
  }
  return clientFD;
}

int Networking::playerInvite() {
  connect();
  // send my name as invitation
  int sz = sizeof(player_name);
  int sent = 0;
  while (sent < sz) {
    sent += send(sktFD, player_name + sent, sz - sent, 0);
    if (sent <= 0) {
      perror("error sending player name\n");
      return -1;
    }
  }
  char ok;
  // recieve the reply
  recv(sktFD, &ok, sizeof(ok), 0);
  if (ok == 'n') {
    return -1;
  } else {
    char guestName[1024];
    // recieve player name
    int totRevd = 0, sz = sizeof(guestName);
    while (totRevd < sz) {
      int rvd = recv(sktFD, guestName + totRevd, sizeof(guestName), 0);
      if (rvd <= 0) {
        perror("Error receiving guest name\n");
        return -1;
      }
      totRevd += rvd;
    }
    strcpy(guest_name, guestName);
  }
  return sktFD;
}

int Networking::playerSearch() {
  char guestName[1024] = {};
  int clientFD = -1;
  char rp;
  bind();
  listen();
  do {
    cout << "Searching for players.." << endl;
    clientFD = accept();
    // recieve player name
    int totRevd = 0, sz = sizeof(guestName);
    while (totRevd < sz) {
      int rvd = recv(clientFD, guestName + totRevd, sizeof(guestName), 0);
      if (rvd <= 0) {
        perror("Error receiving guest name\n");
        return -1;
      }
      totRevd += rvd;
    }
    cout << guestName << " is inviting you to play! " << "(y/n)" << endl;
    cin >> rp;
    // send answer (accept and play or reject and continue searching)
    send(clientFD, &rp, sizeof(rp), 0);
  } while (rp == 'n');
  strcpy(guest_name, guestName);
  // send my name to the guest
  int sz = sizeof(player_name);
  int sent = 0;
  while (sent < sz) {
    sent += send(clientFD, player_name + sent, sz - sent, 0);
    if (sent <= 0) {
      perror("error sending player name\n");
      return -1;
    }
  }
  return clientFD;
}
