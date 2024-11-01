#pragma once
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h> //close(FD)

class Networking{
    private:
        int portnum;
        int sktFD;        //socket file descriptor.
        sockaddr_in addr; //socket address (can be for client or server).
        int search_for_players(); //while listening
    public:                                      
        Networking(int port_number, char* IP) : portnum(port_number), sktFD(socket(AF_INET, SOCK_STREAM, 0))
    {   //specifing address
        addr.sin_family = AF_INET;      //IPv4
        //Converts values between host and network byte order
        addr.sin_port = htons(portnum); 
        addr.sin_addr.s_addr = inet_addr(IP);    
        //use INADDR_ANY to bind the socket to all available interfaces
        /*
            inet_addr converts ip from decimal and dots notation to          
            an integer value suitable for use as an internet address
        */
    }
        int Inv(); //invite player to play. return guest socket file descriptor
        int Lis(); //listen to a port number for invitations. returns client socket file descriptor
};
