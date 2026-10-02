#include "../include/core.hpp"
#include <cstdlib>
#include <iostream>

using namespace std;

int chess_piece::killed[2][6] = {0};

color chess_piece::get_color() { return clr; }

// ---------------------------------------------------------------------------
// Pawn  (in the local view our own pawns always move toward decreasing x)
// ---------------------------------------------------------------------------
bool Pawn::can_reach(spot from, spot to) {
  if (to.x < 0 || to.x >= n || to.y < 0 || to.y >= n)
    return false;
  if (from.x == to.x && from.y == to.y)
    return false;

  // forward one square
  if (from.y == to.y && from.x - to.x == 1)
    return Chess::board[to.x][to.y] == nullptr;

  // forward two squares from the starting rank (must be clear)
  if (from.y == to.y && from.x == 6 && to.x == 4)
    return Chess::board[5][from.y] == nullptr &&
           Chess::board[4][from.y] == nullptr;

  // diagonal capture
  if (from.x - to.x == 1 && abs(from.y - to.y) == 1) {
    chess_piece *target = Chess::board[to.x][to.y];
    return target != nullptr && target->get_color() != clr;
  }
  return false;
}

void Pawn::print_piece() { cout << icons[clr][pawn] << " | "; }
type Pawn::get_type() { return pawn; }

// ---------------------------------------------------------------------------
// Rook
// ---------------------------------------------------------------------------
bool Rook::can_reach(spot from, spot to) {
  if (from.x == to.x && from.y == to.y)
    return false;

  if (from.y == to.y) { // vertical
    int step = (to.x > from.x) ? 1 : -1;
    for (int i = from.x + step; i != to.x; i += step)
      if (Chess::board[i][from.y])
        return false;
    return true;
  }
  if (from.x == to.x) { // horizontal
    int step = (to.y > from.y) ? 1 : -1;
    for (int j = from.y + step; j != to.y; j += step)
      if (Chess::board[from.x][j])
        return false;
    return true;
  }
  return false;
}

void Rook::print_piece() { cout << icons[clr][rook] << " | "; }
type Rook::get_type() { return rook; }

// ---------------------------------------------------------------------------
// Bishop
// ---------------------------------------------------------------------------
bool Bishop::can_reach(spot from, spot to) {
  if (abs(from.x - to.x) != abs(from.y - to.y))
    return false;
  if (from.x == to.x)
    return false;

  int dx = (to.x > from.x) ? 1 : -1;
  int dy = (to.y > from.y) ? 1 : -1;
  for (int i = from.x + dx, j = from.y + dy; i != to.x; i += dx, j += dy)
    if (Chess::board[i][j])
      return false;
  return true;
}

void Bishop::print_piece() { cout << icons[clr][bishop] << " | "; }
type Bishop::get_type() { return bishop; }

// ---------------------------------------------------------------------------
// Knight
// ---------------------------------------------------------------------------
bool Knight::can_reach(spot from, spot to) {
  int dx[] = {-1, -1, 1, 1, 2, 2, -2, -2};
  int dy[] = {2, -2, -2, 2, 1, -1, 1, -1};
  for (int i = 0; i < 8; i++)
    if (from.x + dx[i] == to.x && from.y + dy[i] == to.y)
      return true;
  return false;
}

void Knight::print_piece() { cout << icons[clr][knight] << " | "; }
type Knight::get_type() { return knight; }

// ---------------------------------------------------------------------------
// Queen
// ---------------------------------------------------------------------------
bool Queen::can_reach(spot from, spot to) {
  Bishop b(clr);
  Rook r(clr);
  return b.can_reach(from, to) || r.can_reach(from, to);
}

void Queen::print_piece() { cout << icons[clr][queen] << " | "; }
type Queen::get_type() { return queen; }

// ---------------------------------------------------------------------------
// King
// ---------------------------------------------------------------------------
bool King::can_reach(spot from, spot to) {
  int dx = abs(from.x - to.x);
  int dy = abs(from.y - to.y);
  return (dx <= 1 && dy <= 1 && (dx + dy) > 0);
}

void King::print_piece() { cout << icons[clr][king] << " | "; }
type King::get_type() { return king; }
