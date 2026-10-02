#include "../include/core.hpp"
#include "../include/network_helper.hpp"
#include <cctype>
#include <cstring>
#include <cstdlib>
#include <iostream>

using namespace std;

chess_piece *Chess::board[n][n] = {nullptr};

extern char player_name[1024];
extern char guest_name[1024];

// ---------------------------------------------------------------------------
// Construction / destruction
// ---------------------------------------------------------------------------
Chess::Chess()
    : gstFD(-1), kingspt({7, 4}), mode(good), can_castle_k(true),
      can_castle_q(true), player(white), en_passant({-1, -1}),
      halfmove_clock(0) {}

Chess::~Chess() { CleanUP(); }

// ---------------------------------------------------------------------------
// Networking
// ---------------------------------------------------------------------------
int Chess::Invite_guest(char *gst_IP) {
  player = white;
  in_addr_t gstIP;
  if (inet_pton(AF_INET, gst_IP, &gstIP) != 1) {
    perror("invalid address / Address not supported");
    return -1;
  }
  Networking client(gstIP);
  gstFD = client.playerInvite();
  if (gstFD == -1)
    cout << "Invitation rejected!" << endl;
  else
    cout << "Your guest is " << guest_name << endl;
  return gstFD;
}

void Chess::Search_for_players() {
  player = black;
  Networking host(INADDR_ANY);
  gstFD = host.playerSearch();
  if (gstFD == -1)
    cout << "No guest joined." << endl;
  else
    cout << "Your guest is: " << guest_name << endl;
}

// ---------------------------------------------------------------------------
// Board setup / teardown
// ---------------------------------------------------------------------------
void Chess::init_board() {
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++) {
      delete board[i][j];
      board[i][j] = nullptr;
    }
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 6; j++)
      chess_piece::killed[i][j] = 0;

  kingspt = {7, 4};
  mode = good;
  can_castle_k = can_castle_q = true;
  en_passant = {-1, -1};
  halfmove_clock = 0;
  pos_count.clear();

  color enemy = color(!player);

  for (int j = 0; j < n; j++) {
    board[6][j] = new Pawn(player);
    board[1][j] = new Pawn(enemy);
  }

  board[7][0] = new Rook(player);
  board[7][7] = new Rook(player);
  board[7][1] = new Knight(player);
  board[7][6] = new Knight(player);
  board[7][2] = new Bishop(player);
  board[7][5] = new Bishop(player);
  board[7][3] = new Queen(player);
  board[7][4] = new King(player);

  board[0][0] = new Rook(enemy);
  board[0][7] = new Rook(enemy);
  board[0][1] = new Knight(enemy);
  board[0][6] = new Knight(enemy);
  board[0][2] = new Bishop(enemy);
  board[0][5] = new Bishop(enemy);
  board[0][3] = new Queen(enemy);
  board[0][4] = new King(enemy);
}

void Chess::CleanUP() {
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++) {
      delete board[i][j];
      board[i][j] = nullptr;
    }
  if (gstFD >= 0) {
    close(gstFD);
    gstFD = -1;
  }
  pos_count.clear();
  halfmove_clock = 0;
  mode = good;
}

// ---------------------------------------------------------------------------
// Rendering
// ---------------------------------------------------------------------------
void Chess::Print_Killed(color p) {
  for (int i = 0; i < 6; i++)
    for (int k = 0; k < chess_piece::killed[p][i]; k++)
      cout << icons[p][i] << ' ';
}

void Chess::draw_board() {
  system("clear");

  cout << guest_name << "   ";
  Print_Killed(player);
  cout << "\n\n";

  for (int i = 0; i < n; i++) {
    cout << (n - i) << " | ";
    for (int j = 0; j < n; j++) {
      if (board[i][j])
        board[i][j]->print_piece();
      else
        cout << "  | ";
    }
    cout << "\n   ";
    for (int j = 0; j < n; j++)
      cout << "----";
    cout << "-\n";
  }

  cout << "    ";
  for (char ch = 'a'; ch <= 'h'; ch++)
    cout << ch << "   ";
  cout << "\n\n";

  cout << player_name << "   ";
  Print_Killed(color(!player));
  cout << "\n";
}

// ---------------------------------------------------------------------------
// Rule helpers
// ---------------------------------------------------------------------------
bool Chess::is_en_passant(spot from, spot to) {
  if (en_passant.x == -1)
    return false;
  if (to.x != en_passant.x || to.y != en_passant.y)
    return false;
  if (board[to.x][to.y] != nullptr)
    return false;
  if (from.x < 0 || from.x >= n || from.y < 0 || from.y >= n)
    return false;

  chess_piece *mover = board[from.x][from.y];
  if (!mover || mover->get_type() != pawn)
    return false;
  if (to.x != from.x - 1)          // our pawns move toward decreasing x
    return false;
  if (abs(to.y - from.y) != 1)
    return false;

  chess_piece *victim = board[from.x][to.y];
  if (!victim || victim->get_type() != pawn)
    return false;
  if (victim->get_color() == mover->get_color())
    return false;

  return true;
}

bool Chess::check_castling(spot to) {
  if (kingspt.x != 7 || kingspt.y != 4)
    return false;
  if (to.x != 7)
    return false;
  if (!board[to.x][to.y] || board[to.x][to.y]->get_type() != rook)
    return false;
  if (board[to.x][to.y]->get_color() != player)
    return false;
  if (!safe_spot(kingspt))
    return false;

  if (to.y == 7) { // king side
    if (!can_castle_k)
      return false;
    if (board[7][5] || board[7][6])
      return false;
    if (!safe_spot({7, 5}) || !safe_spot({7, 6}))
      return false;
  } else if (to.y == 0) { // queen side
    if (!can_castle_q)
      return false;
    if (board[7][1] || board[7][2] || board[7][3])
      return false;
    if (!safe_spot({7, 3}) || !safe_spot({7, 2}))
      return false;
  } else {
    return false;
  }
  return true;
}

bool Chess::do_castling(spot to) {
  if (!check_castling(to))
    return false;

  chess_piece *king = board[7][4];
  chess_piece *rook = board[7][to.y];
  board[7][4] = nullptr;
  board[7][to.y] = nullptr;

  if (to.y == 7) {
    board[7][6] = king;
    board[7][5] = rook;
    kingspt = {7, 6};
  } else {
    board[7][2] = king;
    board[7][3] = rook;
    kingspt = {7, 2};
  }
  can_castle_k = can_castle_q = false;
  en_passant = {-1, -1};
  halfmove_clock++;
  return true;
}

bool Chess::check_move(spot from, spot to) {
  if (from.x < 0 || from.y < 0 || to.x < 0 || to.y < 0 || from.x >= n ||
      from.y >= n || to.x >= n || to.y >= n)
    return false;
  if (!board[from.x][from.y])
    return false;
  if (board[from.x][from.y]->get_color() != player)
    return false;
  if (from.x == to.x && from.y == to.y)
    return false;

  // castling?
  if (from.x == kingspt.x && from.y == kingspt.y) {
    chess_piece *tp = board[to.x][to.y];
    if (tp && tp->get_type() == rook && tp->get_color() == player)
      return check_castling(to);
  }

  // attacking own piece
  if (board[to.x][to.y] && board[to.x][to.y]->get_color() == player)
    return false;

  bool ep = is_en_passant(from, to);
  if (!ep && !board[from.x][from.y]->can_reach(from, to))
    return false;

  // ---- simulate the move and verify our own king stays safe ----
  chess_piece *captured = board[to.x][to.y];
  chess_piece *ep_captured = nullptr;
  spot old_kingspt = kingspt;

  if (ep) {
    ep_captured = board[from.x][to.y];
    board[from.x][to.y] = nullptr;
  }
  board[to.x][to.y] = board[from.x][from.y];
  board[from.x][from.y] = nullptr;
  if (board[to.x][to.y]->get_type() == king)
    kingspt = to;

  bool ok = safe_spot(kingspt);

  // ---- undo ----
  board[from.x][from.y] = board[to.x][to.y];
  board[to.x][to.y] = captured;
  if (ep)
    board[from.x][to.y] = ep_captured;
  kingspt = old_kingspt;

  return ok;
}

bool Chess::make_move(spot from, spot to) {
  if (!check_move(from, to))
    return false;
  if (from.x == kingspt.x && from.y == kingspt.y && board[to.x][to.y] &&
      board[to.x][to.y]->get_type() == rook)
    return do_castling(to);
  update_board(from, to);
  return true;
}

void Chess::update_board(spot from, spot to) {
  color enemy = color(!player);
  chess_piece *mover = board[from.x][from.y];
  chess_piece *victim = board[to.x][to.y];

  bool ep = (mover->get_type() == pawn && from.y != to.y && victim == nullptr);
  if (ep) {
    victim = board[from.x][to.y];
    board[from.x][to.y] = nullptr;
  }

  if (victim) {
    chess_piece::killed[enemy][victim->get_type()]++;
    delete victim;
  }
  board[to.x][to.y] = nullptr;
  board[to.x][to.y] = mover;
  board[from.x][from.y] = nullptr;

  // castling rights
  if (mover->get_type() == king) {
    kingspt = to;
    can_castle_k = can_castle_q = false;
  } else if (mover->get_type() == rook) {
    if (from.x == 7 && from.y == 0)
      can_castle_q = false;
    if (from.x == 7 && from.y == 7)
      can_castle_k = false;
  }

  // en-passant target for the opponent
  en_passant = {-1, -1};
  if (mover->get_type() == pawn && abs(to.x - from.x) == 2)
    en_passant = {(from.x + to.x) / 2, from.y};

  // fifty-move counter
  if (mover->get_type() == pawn || victim)
    halfmove_clock = 0;
  else
    halfmove_clock++;

  // promotion
  if (mover->get_type() == pawn && to.x == 0) {
    color c = mover->get_color();
    delete board[to.x][to.y];
    board[to.x][to.y] = new Queen(c);
  }
}

bool Chess::safe_spot(spot spt) {
  if (spt.x < 0 || spt.x >= n || spt.y < 0 || spt.y >= n)
    return true;

  color enemy = color(!player);
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      chess_piece *p = board[i][j];
      if (!p || p->get_color() != enemy)
        continue;
      if (p->get_type() == pawn) {
        if (spt.x - i == 1 && abs(spt.y - j) == 1)
          return false;
      } else {
        if (p->can_reach({i, j}, spt))
          return false;
      }
    }
  }
  return true;
}

bool Chess::can_move() {
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++) {
      chess_piece *p = board[i][j];
      if (!p || p->get_color() != player)
        continue;
      for (int k = 0; k < n; k++)
        for (int u = 0; u < n; u++)
          if (check_move({i, j}, {k, u}))
            return true;
    }
  return false;
}

// ---------------------------------------------------------------------------
// Draw detection helpers
// ---------------------------------------------------------------------------
bool Chess::insufficient_material() {
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++) {
      chess_piece *p = board[i][j];
      if (!p)
        continue;
      type t = p->get_type();
      if (t == pawn || t == rook || t == queen)
        return false;
    }

  int total_minor = 0;
  int bishop_count[2] = {0, 0};
  int bishop_sq[2] = {-1, -1};

  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++) {
      chess_piece *p = board[i][j];
      if (!p)
        continue;
      if (p->get_type() == knight) {
        total_minor++;
      } else if (p->get_type() == bishop) {
        total_minor++;
        bishop_count[p->get_color()]++;
        bishop_sq[p->get_color()] = (i + j) & 1;
      }
    }

  if (total_minor <= 1)
    return true;

  if (total_minor == 2 && bishop_count[white] == 1 &&
      bishop_count[black] == 1 && bishop_sq[white] == bishop_sq[black])
    return true;

  return false;
}

std::string Chess::position_key() {
  std::string s;
  s.reserve(80);
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++) {
      chess_piece *p = board[i][j];
      if (!p) {
        s += '.';
        continue;
      }
      char c;
      switch (p->get_type()) {
      case pawn:   c = 'p'; break;
      case rook:   c = 'r'; break;
      case knight: c = 'n'; break;
      case bishop: c = 'b'; break;
      case queen:  c = 'q'; break;
      default:     c = 'k'; break;
      }
      if (p->get_color() == white)
        c = static_cast<char>(toupper(c));
      s += c;
    }
  s += can_castle_k ? 'K' : '-';
  s += can_castle_q ? 'Q' : '-';
  s += static_cast<char>('0' + (en_passant.x < 0 ? 9 : en_passant.x));
  s += static_cast<char>('0' + (en_passant.y < 0 ? 9 : en_passant.y));
  return s;
}

// ---------------------------------------------------------------------------
// Status
// ---------------------------------------------------------------------------
king_status Chess::update_status() {
  if (mode == win || mode == draw)
    return mode;

  if (insufficient_material())
    return mode = draw;

  if (halfmove_clock >= 100)
    return mode = draw;

  if (++pos_count[position_key()] >= 3)
    return mode = draw;

  bool in_check = !safe_spot(kingspt);
  bool any_piece_move = can_move();

  if (in_check && !any_piece_move) {
    mode = lose; // checkmate
    return mode;
  }
  if (!in_check && !any_piece_move) {
    mode = draw; // stalemate
    return mode;
  }
  return mode = good;
}

// ---------------------------------------------------------------------------
// Wire protocol
// ---------------------------------------------------------------------------
void Chess::sendmv(spot from, spot to) {
  if (gstFD < 0)
    return;
  send_all(gstFD, &from, sizeof(from));
  send_all(gstFD, &to, sizeof(to));
}

void Chess::recvmv() {
  spot from = {-1, -1};
  spot to = {-1, -1};

  if (!recv_all(gstFD, &from, sizeof(from)) ||
      !recv_all(gstFD, &to, sizeof(to))) {
    mode = win;
    return;
  }
  if (from.x == -1) {
    mode = win; // opponent resigned / checkmated
    return;
  }
  if (from.x == -2) {
    mode = draw;
    return;
  }

  from.x = 7 - from.x;
  to.x = 7 - to.x;

  if (from.x < 0 || from.x >= n || from.y < 0 || from.y >= n ||
      to.x < 0 || to.x >= n || to.y < 0 || to.y >= n) {
    mode = win;
    return;
  }
  chess_piece *mover = board[from.x][from.y];
  if (!mover) {
    mode = win;
    return;
  }

  // ---- castling ----
  if (mover->get_type() == king && from.x == 0 && from.y == 4 && to.x == 0 &&
      (to.y == 0 || to.y == 7) && board[0][to.y] &&
      board[0][to.y]->get_type() == rook &&
      board[0][to.y]->get_color() != player) {
    int row = 0;
    if (to.y == 7) {
      board[row][6] = board[row][4];
      board[row][5] = board[row][7];
      board[row][4] = nullptr;
      board[row][7] = nullptr;
    } else {
      board[row][2] = board[row][4];
      board[row][3] = board[row][0];
      board[row][4] = nullptr;
      board[row][0] = nullptr;
    }
    en_passant = {-1, -1};
    halfmove_clock++;
    return;
  }

  chess_piece *victim = board[to.x][to.y];
  bool ep =
      (mover->get_type() == pawn && from.y != to.y && victim == nullptr);
  if (ep) {
    victim = board[from.x][to.y];
    board[from.x][to.y] = nullptr;
  }

  if (victim && victim->get_color() == player) {
    chess_piece::killed[player][victim->get_type()]++;
    if (victim->get_type() == rook) {
      if (to.x == 7 && to.y == 0)
        can_castle_q = false;
      if (to.x == 7 && to.y == 7)
        can_castle_k = false;
    }
    delete victim;
  }
  board[to.x][to.y] = nullptr;
  board[to.x][to.y] = mover;
  board[from.x][from.y] = nullptr;

  en_passant = {-1, -1};
  if (mover->get_type() == pawn && abs(to.x - from.x) == 2)
    en_passant = {(from.x + to.x) / 2, from.y};

  if (mover->get_type() == pawn || victim)
    halfmove_clock = 0;
  else
    halfmove_clock++;

  // promotion
  if (mover->get_type() == pawn && to.x == 7) {
    color c = mover->get_color();
    delete board[to.x][to.y];
    board[to.x][to.y] = new Queen(c);
  }
}
