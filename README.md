# ChessOverSockets

A two-player peer-to-peer chess game over TCP sockets, written in C++17.
One player invites, the other listens, and they play a full game of chess
on the console with no server in between.

![image](https://github.com/abdalrahmanshaban0/ChessOverSockets/assets/126330281/73198e7f-a210-4168-a4a3-c6759330d747)

---

## Table of contents

1. [Build](#1-build)
2. [Starting a game](#2-starting-a-game)
3. [Board and pieces](#3-board-and-pieces)
4. [Turn flow](#4-turn-flow)
5. [Rules implemented](#5-rules-implemented)
6. [Networking](#6-networking)
7. [Wire protocol](#7-wire-protocol)
8. [Source layout](#8-source-layout)
9. [Known limitations](#9-known-limitations)
10. [How to contribute](#10-how-to-contribute)
11. [Possible coming features](#11-possible-coming-features)

---

## 1. Build

```bash
make
```

This produces a `chess` binary in the project root.

---

## 2. Starting a game

Two players connect directly. The **inviter plays white and moves first**;
the **listener plays black**.

### Player A — invite

1. Run `./chess`.
2. Enter your name.
3. Choose option `2` ("Invite a player").
4. Enter the IPv4 address of player B (e.g. `192.168.1.42`).
5. Wait for player B to accept. If rejected, you are prompted to try another address.

### Player B — listen

1. Run `./chess`.
2. Enter your name.
3. Choose option `1` ("Search for players").
4. When an invitation arrives, type `y` to accept or `n` to reject and keep searching.
5. The listening socket uses `SO_REUSEADDR` and is closed as soon as an
   opponent connects, so the same port (8080) is immediately available
   for the next game.

Both players must be able to reach each other on TCP port 8080. On a LAN this
usually needs no configuration; over the internet, forward the port on the
listener's router.

### Menu options

| Option | Meaning |
|---|---|
| `1` | Listen for an incoming invitation (play black). |
| `2` | Invite a peer at a given IPv4 (play white, move first). |
| `0` | Exit the program. |

### During a game

- Type moves in the form `d2 d4` (file letter + rank digit, whitespace-separated).
- Type `resign` on your turn to forfeit.
- The board redraws after every move, and each side's kill list is shown
  under the opponent's name at the top and your own name at the bottom.

---

## 3. Board and pieces

- The board is 8×8, indexed `board[row][column]` with `row = 0` at the top
  (black's back rank) and `row = 7` at the bottom (white's back rank).
- Each cell holds a `chess_piece*` or `nullptr`.
- At the start of every game, `init_board()` deletes and nulls every cell,
  resets the kill counters in `chess_piece::killed[2][6]`, and places the
  standard 32 pieces.
- The local player always sees their own pieces at the bottom. When a move
  arrives over the socket, `recvmv()` flips the row index (`x -> 7 - x`) so
  the two sides can use the same coordinate system internally.
- The kill counter is a `static int[2][6]` shared by every piece class and
  indexed `[color][type]` using the enums in `pieces.hpp`.

Icon table:

```cpp
// icons[color][type]
icons[0] = {"♙", "♖", "♘", "♗", "♔", "♕"};   // white
icons[1] = {"♟︎", "♜", "♞", "♝", "♚", "♛"};   // black
```

`enum color { white, black }` maps white to 0 and black to 1, matching the
table. The inner dimension is 8 to hold the UTF-8 encoding of a piece glyph
plus its variation selector plus a NUL terminator.

---

## 4. Turn flow

The game alternates between two states: **"my turn"** and **"opponent's turn"**.

```
       ┌────────────────────────┐
       │  my turn               │
       │  1. run status check   │
       │  2. read a move or     │
       │     "resign"           │
       │  3. validate + apply   │
       │  4. send the move      │
       └───────────┬────────────┘
                   │
                   ▼
       ┌────────────────────────┐
       │  opponent's turn       │
       │  recvmv() blocks until │
       │  a packet or disconnect│
       └───────────┬────────────┘
                   │
                   └──────► back to "my turn"
```

### Status check (once per turn)

Run at the beginning of every "my turn":

1. **Insufficient material** — K vs K, K + minor vs K, or KB vs KB with the
   two bishops on same-coloured squares. Result: draw.
2. **Fifty-move rule** — 100 halfmoves with no pawn move and no capture.
   Result: draw.
3. **Threefold repetition** — the current position (piece layout + castling
   rights + en-passant target) has appeared three times. Result: draw.
4. **Checkmate** — the king is in check and no legal move exists.
   Result: send `{-1, -1}`, print "Checkmate! You lose.".
5. **Stalemate** — the king is not in check but no legal move exists.
   Result: send `{-2, -2}`, print "Draw.".
6. Otherwise: proceed to read a move.

Status is checked once per turn, not on every `select()` wake-up.

### Making a move

A move `(from, to)` is validated by `check_move()` in this order:

1. Both squares are inside the board.
2. The source square holds a piece belonging to the player.
3. The source and destination squares are not identical.
4. If the source is the king's square and the destination holds a friendly
   rook, the move is handled by `check_castling()`:
   - the rook is the h1 (`{7,7}`) or a1 (`{7,0}`) rook and belongs to us;
   - the corresponding side's castling right is still held;
   - every square between the king and rook is empty;
   - the king is not currently in check;
   - the king does not pass through or land on an attacked square.
5. The destination does not hold a friendly piece.
6. Either the piece's `can_reach()` returns true, or the move is a valid
   en-passant capture.
7. The move is simulated on the board; if the resulting position leaves our
   king in check, the move is rejected and the board is restored exactly.

If all checks pass, `update_board()` commits the move:

- the captured piece is counted in `killed[enemy][type]` and deleted;
- castling rights are revoked if the king or either rook moved;
- the en-passant target square is set if a pawn moved two ranks, otherwise cleared;
- the halfmove clock is reset on a pawn move or capture, otherwise incremented;
- a pawn reaching the last rank is promoted to a queen.

### Receiving a move

`recvmv()` is called on the opponent's turn. It reads exactly two `spot`
structs. Special sentinels:

- `from.x == -1` → opponent resigned or was checkmated. Set `mode = win`.
- `from.x == -2` → opponent declared a draw. Set `mode = draw`.
- Any other value is treated as a move. Coordinates are flipped
  (`x -> 7 - x`), and the same update logic runs, including castling,
  en-passant, promotion, and castling-right revocation on a captured rook.

A closed socket during `recvmv()` sets `mode = win` (the disconnected side
loses).

### Forfeiting

Type `resign` on your turn. The local side sends `{-1, -1}` and the game ends
immediately.

---

## 5. Rules implemented

### Piece movement

| Piece | Movement | Captures |
|---|---|---|
| Pawn | One square forward; two squares from the starting rank if both squares are empty | Diagonally one square forward |
| Knight | L-shape (2+1) | Same as movement |
| Bishop | Any distance diagonally, blocked by pieces | Same as movement |
| Rook | Any distance orthogonally, blocked by pieces | Same as movement |
| Queen | Rook or bishop movement | Same as movement |
| King | One square in any direction | Same as movement |

### Special moves

- **Pawn double-step** — allowed only from the starting rank and only if both
  the intermediate and destination squares are empty.
- **En-passant capture** — the target square is set when a pawn moves two
  ranks and cleared otherwise. A pawn moving diagonally onto an empty square
  that matches the target captures the pawn that just passed it.
- **Castling** — validated by `check_castling()`. Requires: correct rook
  colour, castling right still held for that side, empty path, king not in
  check, and king not passing through or landing on an attacked square.
  The king's and rook's squares are updated atomically in `do_castling()`.
- **Promotion** — a pawn reaching the last rank is replaced by a queen of the
  same colour. Promotion choice is not implemented.

### End conditions

- **Checkmate** — king is in check and no legal move. Losing side sends `{-1,-1}`.
- **Stalemate** — king is not in check but no legal move. Both sides treat it
  as a draw and the detecting side sends `{-2,-2}`.
- **Insufficient material** — K vs K, K + minor vs K, or KB vs KB with
  same-coloured bishops. Result: draw.
- **Fifty-move rule** — 100 halfmoves without a pawn move or capture.
  Result: draw.
- **Threefold repetition** — the same position has appeared three times.
  The position key encodes the piece layout, castling rights, and en-passant
  availability. Result: draw.
- **Resignation** — either side sends `{-1,-1}`.
- **Disconnection** — a closed socket during `recvmv()` ends the game as a
  win for the side that is still connected.

---

## 6. Networking

The game uses a direct TCP connection between the two players. There is no
server or matchmaking. The listener binds to `INADDR_ANY` on port 8080; the
inviter connects to a specified IPv4 address on the same port.

### Reliability

- All writes and reads go through `send_all()` / `recv_all()`, which loop
  until the full buffer is transferred or the socket errors out. This is
  required because TCP `send()` and `recv()` are allowed to transfer fewer
  bytes than requested.
- The listening socket sets `SO_REUSEADDR` so a new game can start
  immediately after the previous one, without waiting for the OS to release
  the port.
- The listening socket is closed as soon as a guest is accepted, releasing
  the port for the next session.
- Every failure path closes the socket it opened:
  - rejected invitation → client socket closed, listener continues;
  - name transfer failure → both sockets closed and the user returns to the menu;
  - successful invitation → listener closed, connected socket returned.

### Address handling

Only IPv4 is supported. Names are sent as fixed-size 1024-byte buffers, with
`strncpy` and explicit NUL-termination on the receiving side to avoid
overflows from malicious or malformed peers.

---

## 7. Wire protocol

Each move is two consecutive `struct spot { int x, y; }` writes:

```
+--------+--------+--------+--------+--------+--------+--------+--------+
| from.x | from.y |  to.x  |  to.y  |   (each int = 4 bytes on x86_64)  |
+--------+--------+--------+--------+--------+--------+--------+--------+
```

Sentinels:

| `from` | Meaning |
|---|---|
| `{-1, -1}` | Resignation or checkmate. Receiver sets `mode = win`. |
| `{-2, -2}` | Draw (stalemate, repetition, 50-move, insufficient material). Receiver sets `mode = draw`. |

There is no handshake, no move acknowledgement, and no versioning. Both
sides trust the other's move packets entirely; illegal moves sent by a
modified client are applied as-is. This is acceptable for a friendly game
between two people who trust each other.

---

## 8. Source layout

```
include/
  core.hpp             Chess class: state, rules, networking entry points
  pieces.hpp           chess_piece base class and the six piece subclasses
  network_helper.hpp   Networking class + send_all/recv_all helpers

src/
  core.cpp             Chess implementation: move validation, board updates,
                       status detection, protocol send/receive
  main.cpp             Menu, turn loop, input parsing, per-game cleanup
  network_helper.cpp   Socket lifecycle: bind, listen, accept, connect
  pieces.cpp           Movement geometry for each piece type
```

Key classes:

- `Chess` — owns the board, both kings' positions, the socket fd, castling
  rights, en-passant target, halfmove clock, and repetition history.
- `chess_piece` — abstract base with `can_reach(from, to)`, `get_type()`,
  `get_color()`, and `print_piece()`. Also owns the static kill counter.
- `Networking` — thin wrapper around `socket`, `bind`, `listen`, `accept`,
  and `connect`. Used only during game setup.
