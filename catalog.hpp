#include <iostream>
#include <cstring>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
using namespace std;

char player_name[1024];
char guest_name[1024];

enum color {white, black};
enum piece {pawn, rook, knight, bishop, king, queen};
enum king_status {good, checkmate, draw, lose, win};
enum action {_move, message};
const char icons[2][6][7] = {
    {"♙", "♖", "♘", "♗", "♔", "♕"},
    {"♟︎", "♜", "♞", "♝", "♚", "♛"},
};

const int chess_pieces_number = 32;
const int chess_board_size = 8;

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
        addr.sin_port = htons(portnum); //Converts values between host and network byte order
        addr.sin_addr.s_addr = inet_addr(IP);    //use INADDR_ANY to bind the socket to all available interfaces
        /*
            inet_addr converts ip from decimal and dots notation to          
            an integer value suitable for use as an internet address
        */
    }
        int Inv(); //invite player to play. return guest socket file descriptor
        int Lis(); //listen to a port number for invitations. returns client socket file descriptor
};

struct spot{
    int x;
    int y;
};

class chess_piece {
    private:
        piece type;
        color player_piece;
    public:
        void init_chess_piece(piece, color);
        void print_piece();
        piece get_type();
        
        bool can_reach(spot, spot, int[8][8]);
        bool pawn_range(spot, spot, int[8][8]);
        bool rook_range(spot, spot, int[8][8]);
        bool knight_range(spot, spot);
        bool bishop_range(spot, spot, int[8][8]);
        bool king_range(spot, spot);

        void check_pawn_promotion();
};

class Chess{
    private:
        int gstFD;  //guest player socket file descriptor
        color player, enemy;   //white or black

        int board[chess_board_size][chess_board_size];    //chess board, each spot contains a index of a piece 
        chess_piece army[chess_pieces_number];  //the one who invites gets white pieces (0) the and other black(1)

        king_status mode;
        bool castling;

        int killed[6];
        int died[6];
    public:
        Chess() : mode(good), castling(1) {
            memset(killed, 0, sizeof(killed));
            memset(died, 0, sizeof(died));
        }
        //Networking (2 ways for starting new game)
        int Invite_guest(int, char*);
        void Search_for_players(int, char*);

        void init_pieces();  //initializes army pieces
        void init_board();   //puts and initializes players pieces on the board
        void draw_board();   //clears console and prints current board 
        
        spot king_pos();
        king_status update_status();

        bool check_move(spot, spot);
        bool check_castling(spot, spot);
        bool make_move(spot, spot);
        bool safe_spot(spot);
        void update_board(spot, spot);
        bool can_move();
        void sendmv(spot, spot);
        void recvmv();
        //features
};

