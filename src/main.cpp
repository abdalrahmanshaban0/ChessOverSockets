#include "../include/core.hpp"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <sys/select.h>
#include <unistd.h>

using namespace std;

char player_name[1024];
char guest_name[1024];

void Print_ASKII_Art() {
  cout << "    _____ _                          \n"
       << "   / ____| |                         \n"
       << "  | |    | |__   ___  ___ ___        \n"
       << "  | |    | '_ \\ / _ \\/ __/ __|       \n"
       << "  | |____| | | |  __/\\__ \\__ \\       \n"
       << "   \\_____|_| |_|\\___||___/___/       \n"
       << "    / __ \\                           \n"
       << "   | |  | |_   _____ _ __            \n"
       << "   | |  | \\ \\ / / _ \\ '__|           \n"
       << "   | |__| |\\ V /  __/ |              \n"
       << "   _\\____/  \\_/ \\___|_|      _       \n"
       << "  / ____|          | |      | |      \n"
       << " | (___   ___   ___| | _____| |_ ___ \n"
       << "  \\___ \\ / _ \\ / __| |/ / _ \\ __/ __|\n"
       << "  ____) | (_) | (__|   <  __/ |_\\__ \\\n"
       << " |_____/ \\___/ \\___|_|\\_\\___|\\__|___/\n";
}

static bool parse_square(const char *s, int &x, int &y) {
  if (!s || strlen(s) < 2)
    return false;
  if (s[0] < 'a' || s[0] > 'h')
    return false;
  if (s[1] < '1' || s[1] > '8')
    return false;
  y = s[0] - 'a';
  x = 8 - (s[1] - '0');
  return true;
}

int main() {
  Print_ASKII_Art();
  cout << "Enter your name: ";
  cin >> player_name;

  while (true) {
    int op = 0;
    cout << "\n1- Search for players\n2- Invite a player\n0- Quit\nEnter (0,1,2): ";
    if (!(cin >> op))
      break;
    if (op == 0)
      break;
    if (op != 1 && op != 2) {
      cout << "Enter a valid choice!" << endl;
      continue;
    }

    {
      Chess game;

      if (op == 1) {
        game.Search_for_players();
      } else {
        int chk = -1;
        while (chk == -1) {
          cout << "Enter the player IPv4: ";
          char IP[64];
          if (!(cin >> IP))
            return 0;
          chk = game.Invite_guest(IP);
        }
      }
      if (game.getSocket() < 0)
        continue;

      game.init_board();
      game.draw_board();

      // op == 2  -> we invited -> we play white -> we move first
      bool my_turn = (op == 2);
      bool need_status = true;

      while (true) {
        // ------------------------------------------------------------
        // Opponent's turn
        // ------------------------------------------------------------
        if (!my_turn) {
          cout << guest_name << "'s turn. Waiting..." << endl;
          game.recvmv();

          king_status m = game.getMode();
          if (m == win) {
            cout << "You win!" << endl;
            break;
          }
          if (m == draw) {
            cout << "Draw." << endl;
            break;
          }
          game.draw_board();
          my_turn = true;
          need_status = true;
          continue;
        }

        // ------------------------------------------------------------
        // Our turn: status check
        // ------------------------------------------------------------
        if (need_status) {
          king_status st = game.update_status();
          if (st == lose) {
            cout << "Checkmate! You lose." << endl;
            game.sendmv({-1, -1}, {-1, -1});
            break;
          }
          if (st == draw) {
            cout << "Draw." << endl;
            game.sendmv({-2, -2}, {-2, -2});
            break;
          }
          need_status = false;
        }

        cout << "Enter your move (e.g. d2 d4), or 'resign': ";
        cout.flush();

        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);
        FD_SET(game.getSocket(), &rfds);
        int mx = max(STDIN_FILENO, game.getSocket()) + 1;

        if (select(mx, &rfds, nullptr, nullptr, nullptr) < 0) {
          perror("select");
          break;
        }

        if (FD_ISSET(game.getSocket(), &rfds)) {
          game.recvmv();
          king_status m = game.getMode();
          if (m == win) {
            cout << "You win!" << endl;
            break;
          }
          if (m == draw) {
            cout << "Draw." << endl;
            break;
          }
          game.draw_board();
          my_turn = false;
          continue;
        }

        if (FD_ISSET(STDIN_FILENO, &rfds)) {
          char f[16] = {0}, t[16] = {0};
          if (!(cin >> f))
            break;

          if (strcmp(f, "resign") == 0) {
            cout << "You resigned." << endl;
            game.sendmv({-1, -1}, {-1, -1});
            break;
          }
          if (!(cin >> t))
            break;

          int x1, y1, x2, y2;
          if (!parse_square(f, x1, y1) || !parse_square(t, x2, y2)) {
            cout << "Invalid coordinates. Use e.g. d2 d4" << endl;
            continue;
          }
          if (!game.make_move({x1, y1}, {x2, y2})) {
            cout << "Illegal move!" << endl;
            continue;
          }
          game.draw_board();
          game.sendmv({x1, y1}, {x2, y2});
          my_turn = false;
        }
      }

      game.CleanUP();
    }
  }
  return 0;
}
