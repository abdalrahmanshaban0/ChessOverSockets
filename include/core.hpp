#pragma once
#include "pieces.hpp"
#include <map>
#include <string>

enum king_status { good, checkmate, draw, lose, win };

// icons[color][type]  ->  color: 0 = white, 1 = black
const char icons[2][6][8] = {{"♙", "♖", "♘", "♗", "♔", "♕"},
                             {"♟︎", "♜", "♞", "♝", "♚", "♛"}};

const int n = 8;

class Chess {
private:
  int gstFD;                 // guest player socket file descriptor
  spot kingspt;              // local player's king square
  king_status mode;
  bool can_castle_k;         // local player still has king-side right
  bool can_castle_q;         // local player still has queen-side right
  color player;              // local player's colour
  spot en_passant;           // en-passant target square, {-1,-1} if none
  int halfmove_clock;        // fifty-move rule counter
  std::map<std::string, int> pos_count; // threefold repetition counter

  bool check_castling(spot rook_sq);
  bool is_en_passant(spot from, spot to);
  bool insufficient_material();
  std::string position_key();
  bool is_pinned(int ax, int ay, spot king, color pinner);

public:
  static chess_piece *board[n][n];

  Chess();
  ~Chess();

  int getSocket() const { return gstFD; }
  king_status getMode() const { return mode; }
  color getPlayer() const { return player; }

  // Networking
  int Invite_guest(char *);
  void Search_for_players();

  void init_board();
  void CleanUP();
  void draw_board();
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
};
