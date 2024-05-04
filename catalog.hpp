#include <iostream>
#include <cstring>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h> //close(FD)
using namespace std;

char player_name[1024];
char guest_name[1024];

enum color {white, black};
enum type {pawn, rook, knight, bishop, king, queen};
enum king_status {good, checkmate, draw, lose, win};
const char icons[2][6][7] = {
    {"♟︎", "♜", "♞", "♝", "♚", "♛"},
    {"♙", "♖", "♘", "♗", "♔", "♕"}
};

const int n = 8;

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

struct spot{
    int x, y;
};
class chess_piece;

class Chess{
    private:
        int gstFD;  //guest player socket file descriptor
        int killed[6];
        int died[6];      
        spot kingspt;
        king_status mode;
        bool castling;
        color player;   //Who invites gets white the other gets black
    public:
        static chess_piece* board[n][n];    //Each spot contains a pointer to piece
        
        Chess() : mode(good), castling(1), kingspt({7,4}) {
            memset(killed, 0, sizeof(killed));
            memset(died, 0, sizeof(died));
        }
        Chess(int dummyArg) { }
        //Networking (2 ways for starting new game)
        int Invite_guest(int, char*);
        void Search_for_players(int, char*);

        void init_board();   //puts and initializes players pieces on the board
        void CleanUP();
        void draw_board();   //clears console and prints current board 

        king_status update_status();
        bool make_move(spot, spot);
        bool check_move(spot, spot);
        bool do_castling(spot);
        bool safe_spot(spot);
        bool can_move();
        void update_board(spot, spot);
        void sendmv(spot, spot);
        void recvmv();
        //features
};
chess_piece* Chess::board[n][n] = {NULL};

class chess_piece {
    protected:
        color clr;
    public:
        chess_piece(color c) : clr(c) { }
        virtual ~chess_piece() { }
        color get_color();
        virtual void print_piece() = 0;
        virtual type get_type() = 0;
        virtual bool can_reach(spot, spot) = 0;
};

class Pawn : public chess_piece {
    public:
        Pawn(color clr) : chess_piece(clr){ }
        void print_piece();
        type get_type();
        bool can_reach(spot, spot);
};
class Rook : public chess_piece {
    public:
        Rook(color clr) : chess_piece(clr) { }
        void print_piece();
        type get_type();
        bool can_reach(spot, spot);
};
class Knight : public chess_piece {
    public:
        Knight(color clr) : chess_piece(clr) { }
        void print_piece();
        type get_type();
        bool can_reach(spot, spot);
};
class Bishop : public chess_piece {
    public:
        Bishop(color clr) : chess_piece(clr) { }
        void print_piece();
        type get_type();
        bool can_reach(spot, spot);
};
class King : public chess_piece {
    public:
        King(color clr) : chess_piece(clr) { }
        void print_piece();
        type get_type();
        bool can_reach(spot, spot);
};
class Queen : public chess_piece {
    public:
        Queen(color clr) : chess_piece(clr) { }
        void print_piece();
        type get_type();
        bool can_reach(spot, spot);
};

