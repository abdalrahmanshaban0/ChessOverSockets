#include "moves.hpp"
int Chess::Invite_guest(int gst_port, char* gst_IP){
    player = white;
    Networking client(gst_port, gst_IP);
    gstFD = client.Inv();
    if(gstFD == -1){
        cout << "Invitation rejected!" << endl;
    }
    else cout << "Your guest is " << guest_name << endl;
    return gstFD;
}
void Chess::Search_for_players(int my_port, char* myinterfaceIP){
    player = black;
    Networking host(my_port, myinterfaceIP);
    gstFD = host.Lis();    
    cout << "Your guest is: " << guest_name << endl;
}
//*******************************
void Chess::init_board(){
    color enemy = color(!player);
    for(int j = 0; j < n; j++){
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
void Chess::Print_Killed(color p){
    for(int i = 0; i < 6; i++){
        if(chess_piece::killed[p][i] && i == 0){
            cout << chess_piece::killed[p][i] << 'x' << icons[p][i] << ' ';
        }
        else{
            int tmp  = chess_piece::killed[p][i];
            while(tmp--)
                cout << icons[player][i] << ' ';
        }
    }
}
void Chess::draw_board(){
    system("clear");
    cout << guest_name << ' ';
    Print_Killed(player);
    cout << endl;
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            if(j == 0){
                cout << n-i << ' ' << "| ";
            }
            if(board[i][j]){
                board[i][j]->print_piece();
            }
            else{
                cout << "  | ";
            }
        }
        cout << endl;
        for(int j = 0; j < 4*n+3; j++){
            if(j > 1)
                cout << '-';
            else 
                cout << ' ';
        }
        cout << endl;
    }
    cout << "    ";
    for(char ch = 'a'; ch <= 'h'; ch++) cout << ch << "   "; cout << endl;
    cout << player_name << ' ';
    Print_Killed(color(!player)); 
    cout << endl;
}
//*******************************
bool Chess::make_move(spot from, spot to){
    if(check_move(from, to)){
        if(from.x == kingspt.x && from.y == kingspt.y && board[to.x][to.y] && board[to.x][to.y]->get_type() == rook){
            return do_castling(to);
        }
        update_board(from, to);
        return 1;
    }
    return 0;
}

void Chess::update_board(spot from, spot to){
    if(board[to.x][to.y] && board[to.x][to.y]->get_color() == !player){
        chess_piece::killed[!player][board[to.x][to.y]->get_type()]++;
        delete board[to.x][to.y];
        board[to.x][to.y] = NULL;
    }
    swap(board[from.x][from.y], board[to.x][to.y]);
    if(board[to.x][to.y]->get_type() == king){
        castling = 0;
        kingspt = to;
    }
    else if(to.x == 0 && board[to.x][to.y]->get_type() == pawn){
        delete board[to.x][to.y];
        board[to.x][to.y] = new Queen(player);
    }
}
bool Chess::check_move(spot from, spot to){
    //out of bounds
    if(from.x < 0 || from.y < 0 || to.x < 0 || to.y < 0 || from.x >= 8 || from.y >= 8 || to.x >= 8 || to.y >= 8)
        return 0;
    //moving empty spot
    if(board[from.x][from.y] == NULL) return 0;
    //moving enemey's piece
    color enemy = color(!player);
    if(board[from.x][from.y]->get_color() == enemy) return 0;
    //check castling
    bool sgn = 0;
    if(from.x == kingspt.x && from.y == kingspt.y && board[to.x][to.y] && board[to.x][to.y]->get_type() == rook){
        if(!castling) return 0;
        return 1;
    }
    //attacking allied piece
    if(board[to.x][to.y] && board[to.x][to.y]->get_color() == player) return 0;
    //not a valid piece move
    if(!board[from.x][from.y]->can_reach(from, to)) return 0;


    chess_piece* toptr = board[to.x][to.y];
    //any killed 
    if(board[to.x][to.y] && board[to.x][to.y]->get_color() == enemy){
        board[to.x][to.y] = NULL;
    }
    //try to make the move and check the king if is safe
    swap(board[from.x][from.y], board[to.x][to.y]);
    spot temp = kingspt;
    if(board[to.x][to.y]->get_type() == king) kingspt = to;
    bool ok = 1;
    if(!safe_spot(kingspt)){
        ok = 0;
    }
    swap(board[from.x][from.y], board[to.x][to.y]);
    board[to.x][to.y] = toptr;
    kingspt = temp;
    return ok;
}
bool Chess::do_castling(spot to){
    if(!castling) return 0;
    if(kingspt.x == 7 && kingspt.y == 4 && (to.x == 7 && (to.y == 0 || to.y == 7))){
        if(kingspt.y < to.y){
            for(int j = kingspt.y+1; j < to.y; j++){
                if(board[7][j]) return 0;
            }
            swap(board[kingspt.x][kingspt.y], board[7][6]);
            swap(board[to.x][to.y], board[7][5]);
            kingspt = {7, 6};
            if(!safe_spot(kingspt)){
                swap(board[kingspt.x][kingspt.y], board[7][6]);
                swap(board[to.x][to.y], board[7][5]);
                kingspt = {7, 4};
                return 0;
            }
        }
        else{
            for(int j = kingspt.y-1; j > to.y; j--){
                if(board[7][j]) return 0;
            }
            swap(board[kingspt.x][kingspt.y], board[7][2]);
            swap(board[to.x][to.y], board[7][3]);
            kingspt = {7, 2};
            if(!safe_spot(kingspt)){
                swap(board[kingspt.x][kingspt.y], board[7][2]);
                swap(board[to.x][to.y], board[7][3]);
                kingspt = {7, 4};
                return 0;
            }
        }
        castling = 0;
        return 1;
    }
    return 0;
}
bool Chess::safe_spot(spot spt){
    color enemy = color(!player);
    for(int i = 0 ;i < n; i++){
        for(int j = 0; j < n; j++){
            if(board[i][j] && board[i][j]->get_color() == enemy){ //enemy
                if(board[i][j]->get_type() != pawn){
                    if(board[i][j]->can_reach({i,j}, spt)) return 0;
                }
                else{
                    if(spt.x-i == 1 && abs(spt.y-j) == 1) return 0;
                }
            }
        }
    }
    return 1;
}
king_status Chess::update_status(){
    if(mode == win || mode == draw) return mode;
    bool any_piece_move = can_move();
    if(!safe_spot(kingspt)){
        if(any_piece_move) { 
            castling = 0;
            return mode = checkmate;
        }
        CleanUP();
        return lose;
    }
    else{
        if(!any_piece_move) {CleanUP(); return mode = draw;}
    }
    return mode = good;
}
bool Chess::can_move(){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            if(board[i][j] && board[i][j]->get_color() == player)
                for(int k = 0; k < n; k++){
                    for(int u = 0; u < n; u++){
                        if(check_move({i,j},{k,u})){
                            return 1;
                        }
                    }
                }
        }
    }
    return 0;
}
//********************************
void Chess::sendmv(spot from, spot to){
    send(gstFD, &from, sizeof(from), 0);
    send(gstFD, &to, sizeof(to), 0);
}
void Chess::recvmv(){
    spot from = {-1, -1}, to = {-1, -1};
    recv(gstFD, &from, sizeof(from), 0);
    recv(gstFD, &to, sizeof(to), 0);
    if(from.x == -1){
        mode = win;
        return;
    }
    else if(from.x == -2){
        mode = draw;
        return;
    }
    from.x = 7-from.x;
    to.x = 7-to.x;

    //castling case
    if(board[from.x][from.y]->get_type() == king && board[to.x][to.y] && board[to.x][to.y]->get_type() == rook){
        if(from.y < to.y){
            swap(board[from.x][from.y], board[0][6]);
            swap(board[to.x][to.y], board[0][5]);
        }
        else{
            swap(board[from.x][from.y], board[0][2]);
            swap(board[to.x][to.y], board[0][3]);
        }
        return;
    }

    if(board[to.x][to.y] && board[to.x][to.y]->get_color() == player){ //my piece died!
        chess_piece::killed[player][board[to.x][to.y]->get_type()]++;
        delete board[to.x][to.y];
        board[to.x][to.y] = NULL;
    }
    swap(board[from.x][from.y], board[to.x][to.y]);
    if(to.x == 7 && board[to.x][to.y]->get_type() == pawn){
        delete board[to.x][to.y];
        color enemy = color(!player);
        board[to.x][to.y] = new Queen(enemy);
    }
}
void Chess::CleanUP(){
    for(int i = 0 ;i < n; i++){
        for(int j = 0; j < n; j++){
            if(board[i][j]) delete board[i][j];
        }
    }
    close(gstFD);
}
