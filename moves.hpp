#include "catalog.hpp"
color chess_piece::get_color() {
    return clr;
}
bool Pawn::can_reach(spot from, spot to) {
    if(board[to.x][to.y] && board[to.x][to.y]->get_color() == !player){ //attacking
        if(from.x-to.x == 1 && abs(from.y-to.y) == 1) return 1;
    }
    else{  //moving
        if(from.y == to.y){
            if(from.x-to.x == 1) return 1;
            if(from.x == 6 && from.x - to.x == 2) return 1;
        }
    }
    return 0;
}
void Pawn::print_piece(){
    cout << icons[clr][pawn] << " | ";
}
type Pawn::get_type(){
    return pawn;
}

bool Rook::can_reach(spot from, spot to){
    if(from.y == to.y){ //up and down
        if(from.x > to.x){ //up
            for(int i = from.x-1; i > to.x; i--){
                if(board[i][from.y]) return 0;
            }
            return 1;
        }
        else if(from.x < to.x){ //down
            for(int i = from.x+1; i < to.x; i++){
                if(board[i][from.y]) return 0;
            }
            return 1;
        }
    }
    else if(from.x == to.x){ //left and right
        if(from.y < to.y){ //right
            for(int i = from.y+1; i < to.y; i++){
                if(board[from.x][i]) return 0;
            }
            return 1;
        }
        else if(from.y > to.y){ //left
            for(int i = from.y-1; i > to.y; i--){
                if(board[from.x][i]) return 0;
            }
            return 1;
        }
    }
    return 0;
}
void Rook::print_piece(){
    cout << icons[clr][rook] << " | ";
}
type Rook::get_type(){
    return rook;
}

bool Bishop::can_reach(spot from, spot to){
    if(abs(from.x-to.x) != abs(from.y-to.y)) return 0;
    if(from.x > to.x && from.y < to.y){ //dia_up_right
        for(int i = from.x-1, j = from.y+1; i > to.x && j < to.y; i--, j++){
            if(board[i][j]) return 0;
        }
        return 1;
    }
    else if(from.x > to.x && from.y > to.y){ //dia_up_left
        for(int i = from.x-1, j = from.y-1; i > to.x && j > to.y; i--, j--){
            if(board[i][j]) return 0;
        }
        return 1;
    }
    else if(from.x < to.x && from.y < to.y){ //dia_down_right
        for(int i = from.x+1, j = from.y+1; i < to.x && j < to.y; i++, j++){
            if(board[i][j]) return 0;
        }
        return 1;
    }
    else if(from.x < to.x && from.y > to.y){ //dia_down_left
        for(int i = from.x+1, j = from.y-1; i < to.x && j > to.y; i++, j--){
            if(board[i][j]) return 0;
        }
        return 1;
    }
    return 0;
}
void Bishop::print_piece(){
    cout << icons[clr][bishop] << " | ";
}
type Bishop::get_type(){
    return bishop;
}

bool Knight::can_reach(spot from, spot to){
    int dx[] = {-1, -1, 1, 1, 2, 2, -2, -2};
    int dy[] = {2, -2, -2, 2, 1, -1, 1, -1};
    for(int i = 0; i < 8; i++){
        if(from.x+dx[i] == to.x && from.y+dy[i] == to.y) return 1;
    }
    return 0;
}
void Knight::print_piece(){
    cout << icons[clr][knight] << " | ";
}
type Knight::get_type(){
    return knight;
}

bool Queen::can_reach(spot from, spot to){
    Bishop temp1(clr);
    Rook temp2(clr);
    return (temp1.can_reach(from, to) || temp2.can_reach(from, to));
}
void Queen::print_piece(){
    cout << icons[clr][queen] << " | ";
}
type Queen::get_type(){
    return queen;
}

bool King::can_reach(spot from, spot to){
    int dx[] = {-1, 1, 0, 0, 1, 1, -1, -1};
    int dy[] = {0, 0, 1, -1, 1, -1, 1, -1};
    for(int i = 0 ;i < 8; i++){
        if(from.x+dx[i] == to.x && from.y+dy[i] == to.y){
            return 1;
        }
    }
    return 0;
}
void King::print_piece(){
    cout << icons[clr][king] << " | ";
}
type King::get_type(){
    return king;
}
