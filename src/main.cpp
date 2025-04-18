#include "../include/core.hpp"
#include <iostream>
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

int main() {
  Print_ASKII_Art();
  cout << "Enter your name: ";
  cin >> player_name;
  int op;
  while (true) {
    cout << "1- Search for players\n2- Invite a player\nEnter (1,2): ";
    cin >> op;
    Chess game;
    if (op == 1) {
      game.Search_for_players();
    } else if (op == 2) {
      int chk = -1;
      while (chk == -1) {
        cout << "Enter the player IPv4: " << endl;
        char IP[15];
        cin >> IP;
        chk = game.Invite_guest(IP);
      }
    } else {
      cout << "Enter a valid choice!" << endl;
      continue;
    }
    game.init_board();
    game.draw_board();
    bool ord = (op == 1 ? 1 : 0);
    bool f = 1;
    while (true) {
      if (ord) {
        if (f) {
          king_status myst = game.update_status();
          if (myst == lose) {
            cout << "You lose!" << endl;
            game.sendmv({-1, -1}, {-1, -1});
            break;
          } else if (myst == win) {
            cout << "You win!" << endl;
            break;
          } else if (myst == draw) {
            cout << "draw" << endl;
            game.sendmv({-2, -2}, {-2, -2});
            break;
          }
          f = 0;
        }
        cout << "Enter a move in form like: d2 d4" << endl;
        char from[3], to[3];
        cin >> from >> to;
        int y1 = from[0] - 'a', x1 = (8 - (from[1] - '0'));
        int y2 = to[0] - 'a', x2 = (8 - (to[1] - '0'));
        bool ok = game.make_move({x1, y1}, {x2, y2});
        if (ok) {
          f = 1;
          game.draw_board();
          game.sendmv({x1, y1}, {x2, y2});
          ord = !ord;
        } else {
          cout << "Not a valid move!" << endl;
        }
      } else {
        cout << guest_name << " turn" << endl;
        game.recvmv();
        game.draw_board();
        ord = !ord;
      }
    }
  }
  return 0;
}
