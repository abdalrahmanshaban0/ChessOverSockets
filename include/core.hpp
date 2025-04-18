#pragma once
#include "pieces.hpp"
enum king_status {good, checkmate, draw, lose, win};
const char icons[2][6][7] = {
    {"♟︎", "♜", "♞", "♝", "♚", "♛"},
    {"♙", "♖", "♘", "♗", "♔", "♕"}
};

const int n = 8;

class Chess{
    private:
        int gstFD;  //guest player socket file descriptor
        spot kingspt;
        king_status mode;
        bool castling;
        color player;   //Who invites gets white the other gets black
    public:
        static chess_piece* board[n][n];    //Each spot contains a pointer to piece
        
        Chess() : mode(good), castling(1), kingspt({7,4}) { }
        //Networking (2 ways for starting new game)
        int Invite_guest(char*);
        void Search_for_players();

        void init_board();   //puts and initializes players pieces on the board
        void CleanUP();
        void draw_board();   //clears console and prints current board 
        void Print_Killed(color);

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

