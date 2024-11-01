#include "../include/network_stuff.hpp"
#include <iostream>
using namespace std;

extern char player_name[1024];
extern char guest_name[1024];

int Networking::Inv(){
    //sending connection request 
    int ok_connect = connect(sktFD, (sockaddr*) &addr, sizeof(addr));
    if(ok_connect == -1){
        cout << "Error connecting to the address!"; return -1;
    }
    //send my name as invitation
    send(sktFD, player_name, sizeof(player_name), 0);
    char ok;
    //recieve the reply
    recv(sktFD, &ok, sizeof(ok), 0);
    if(ok == 'n'){
        return -1;
    }
    else{
        char buffer[1024];
        //recieve player name
        recv(sktFD, buffer, sizeof(buffer), 0);
        strcpy(guest_name, buffer);
    }
    return sktFD;
}

int Networking::search_for_players(){
    char buffer[1024] = {};
    int clientFD = -1;
    char rp;
    do{
        //Accept connection
        clientFD = accept(sktFD, nullptr, nullptr);
        //recieve player name
        recv(clientFD, buffer, sizeof(buffer), 0);
        cout << buffer << " is inviting you to play! " << " (y/n)" << endl;
        cin >> rp;
        //send answer (accept and play or reject and continue searching)
        send(clientFD, &rp, sizeof(rp), 0);
    } while(rp == 'n');
    strcpy(guest_name, buffer);
    send(clientFD, player_name, sizeof(player_name), 0);
    return clientFD;
}

int Networking::Lis(){
    //bind a socket
    int ok_bind = bind(sktFD, (sockaddr*) &addr, sizeof(addr));
    if(ok_bind == -1){
        cout << "Error binding a socket!"; return -1;
    }
    //listening to assigned socket
    int ok_listen = listen(sktFD, 5);
    if(ok_listen == -1){
        cout << "Error listening to the socket!"; return -1;
    }
    //player found
    cout << "Searching for players.." << endl;
    int clientFD = search_for_players();
    return clientFD;
}
