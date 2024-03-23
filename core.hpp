#include "catalog.hpp"
#include "moves.hpp"
int Chess::Invite_guest(int gst_port, char* gst_IP){
    player = white;
    enemy = black;
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
    enemy = white;
    Networking host(my_port, myinterfaceIP);
    gstFD = host.Lis();    
    cout << "Your guest is: " << guest_name << endl;
}
//*******************************
void Chess::init_pieces(){ 
    piece arr[16] =
    {
        pawn, pawn,   pawn,   pawn,  pawn, pawn,   pawn,   pawn,
        rook, knight, bishop, queen, king, bishop, knight, rook
    };
    int i = 0;
    for(; i < 16; i++){
        army[i].init_chess_piece(arr[i], player);
    }
    for(int j = 0; j < 16; j++, i++){
        army[i].init_chess_piece(arr[j], enemy);
    }
}
void chess_piece::init_chess_piece(piece typ, color clr){
    type = typ;
    player_piece = clr;
}
void Chess::init_board(){
    int it = 0;
    memset(board, -1, sizeof(board));
    for(int i = 6; i < chess_board_size; i++){    //last 2 rows
        for(int j = 0; j < chess_board_size; j++){
            board[i][j] = it++;
        }
    }
    for(int i = 1; i >= 0; i--){
        for(int j = 0; j < chess_board_size; j++){
            board[i][j] = it++;
        }
    }
}

void Chess::draw_board(){
    system("clear");
    cout << guest_name << ' ';
    for(int i = 0; i < 6; i++){
        if(died[i] && i == 0){
            cout << died[i] << 'x' << icons[player][i] << ' ';
        }
        else{
            int tmp  = died[i];
            while(tmp--)
                cout << icons[player][i] << ' ';
        }
    }
    cout << endl;
    for(int i = 0; i < chess_board_size; i++){
        for(int j = 0; j < chess_board_size; j++){
            if(j == 0){
                cout << chess_board_size-i << ' ' << "| ";
            }
            if(board[i][j] != -1){
                army[board[i][j]].print_piece();
            }
            else{
                cout << "  | ";
            }
        }
        cout << endl;
        for(int j = 0; j < 4*chess_board_size+3; j++){
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
    for(int i = 0; i < 6; i++){
        if(killed[i] && i == 0){
            cout << killed[i] << 'x' << icons[enemy][i] << ' ';
        }
        else{
            int tmp = killed[i];
            while(tmp--)
                cout << icons[enemy][i] << ' ';
        }
    }
    cout << endl;
}
void chess_piece::print_piece(){
    cout << icons[player_piece][type] << " | ";
}
piece chess_piece::get_type(){
    return type;
}
//*******************************

spot Chess::king_pos(){
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(board[i][j] == 12){
                return {i, j};
            }
        }
    }
    return {-1, -1};
}

bool Chess::can_move(){
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(board[i][j] > -1 && board[i][j] < 16)
                for(int k = 0; k < 8; k++){
                    for(int u = 0; u < 8; u++){
                        if(check_move({i,j},{k,u})){
                            return 1;
                        }
                    }
                }
        }
    }
    return 0;
}

king_status Chess::update_status(){
    if(mode == win || mode == draw) return mode;
    bool any_piece_move = can_move();
    if(!safe_spot(king_pos())){
        if(any_piece_move) { 
            castling = 0;
            return mode = checkmate;
        }
        return lose;
    }
    else{
        if(!any_piece_move) return mode = draw;
    }
    return mode = good;
}

void chess_piece::check_pawn_promotion(){
    if(type == pawn)
        type = queen; 
}

void Chess::update_board(spot from, spot to){
    if(board[to.x][to.y] >= 16){
        killed[army[board[to.x][to.y]].get_type()]++;
        board[to.x][to.y] = -1;
    }
    if(army[board[from.x][from.y]].get_type() == king){
        castling = 0;
    }
    swap(board[from.x][from.y], board[to.x][to.y]);
    for(int j = 0; j < 8; j++){
        army[board[0][j]].check_pawn_promotion();
        army[board[7][j]].check_pawn_promotion();
    }
}

bool Chess::make_move(spot from, spot to){
    if(check_move(from, to)){
        update_board(from, to);
        return 1;
    }
    return 0;
}

bool Chess::check_castling(spot from, spot to){
    if(!castling) return 0;
    spot chk = king_pos();
    if(chk.x == 7 && chk.y == 4 && (to.x == 7 && (to.y == 0 || to.y == 7))){
        if(chk.y < to.y){
            for(int j = chk.y+1; j < to.y; j++){
                if(board[7][j] != -1) return 0;
            }
            swap(board[from.x][from.y], board[7][6]);
            swap(board[to.x][to.y], board[7][5]);
            if(!safe_spot(king_pos())){
                swap(board[from.x][from.y], board[7][6]);
                swap(board[to.x][to.y], board[7][5]);
                return 0;
            }
        }
        else{
            for(int j = chk.y-1; j > to.y; j--){
                if(board[7][j] != -1) return 0;
            }
            swap(board[from.x][from.y], board[7][2]);
            swap(board[to.x][to.y], board[7][3]);
            if(!safe_spot(king_pos())){
                swap(board[from.x][from.y], board[7][2]);
                swap(board[to.x][to.y], board[7][3]);
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

bool Chess::check_move(spot from, spot to){
    //out of bounds
    if(from.x < 0 || from.y < 0 || to.x < 0 || to.y < 0 || from.x >= 8 || from.y >= 8 || to.x >= 8 || to.y >= 8)
        return 0;
    //moving enemey's piece
    if(board[from.x][from.y] >= 16) return 0;
    //moving empty spot
    if(board[from.x][from.y] == -1)
        return 0;
    bool sgn = 0;
    if(army[board[from.x][from.y]].get_type() == king && army[board[to.x][to.y]].get_type() == rook){
        if(!check_castling(from, to)) return 0;
        return 1;
    }
    //attacking allied piece
    if(board[to.x][to.y] > -1 && board[to.x][to.y] < 16) return 0;
    //not a valid piece move
    if(!army[board[from.x][from.y]].can_reach(from, to, board)) return 0;

    int tempto = board[to.x][to.y];
    if(board[to.x][to.y] >= 16){
        board[to.x][to.y] = -1;
    }
    swap(board[from.x][from.y], board[to.x][to.y]);

    bool ok = 1;
    if(!safe_spot(king_pos())){
        ok = 0;
    }
    swap(board[from.x][from.y], board[to.x][to.y]);
    board[to.x][to.y] = tempto;
    return ok;
}
//********************************
bool chess_piece::can_reach(spot from, spot to, int board[8][8]){
    switch (type){
        case pawn:
            return pawn_range(from, to, board);
            break;
        case rook:
            return rook_range(from, to, board);
            break;
        case bishop:
            return bishop_range(from, to, board);
            break;
        case knight:
            return knight_range(from, to);
            break;
        case queen:
            return (rook_range(from, to, board) || bishop_range(from, to, board));
            break;
        case king:
            return king_range(from, to);
            break;
    }
    return 0;
}

bool Chess::safe_spot(spot spt){
    for(int i = 0 ;i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(board[i][j] >= 16){ //enemy
                if(army[board[i][j]].get_type() != pawn){
                    if(army[board[i][j]].can_reach({i,j}, spt, board)) return 0;
                }
                else{
                    if(spt.x-i == 1 && abs(spt.y-j) == 1) return 0;
                }
            }
        }
    }
    return 1;
}

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
    if(army[board[from.x][from.y]].get_type() == king && army[board[to.x][to.y]].get_type() == rook){
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

    if(board[to.x][to.y] > -1 && board[to.x][to.y] < 16){ //my piece died!
        died[army[board[to.x][to.y]].get_type()]++;
        board[to.x][to.y] = -1;
    }
    swap(board[from.x][from.y], board[to.x][to.y]);
    if(to.x == 0 || to.x == 7)
        army[board[to.x][to.y]].check_pawn_promotion();
}
