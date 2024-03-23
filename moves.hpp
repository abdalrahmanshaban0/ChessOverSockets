bool chess_piece::pawn_range(spot from, spot to, int board[8][8]){
    if(board[to.x][to.y] >= 16){ //attacking
        if(from.x-to.x == 1 && abs(from.y-to.y) == 1) return 1;
    }
    else{
        if(from.y == to.y){
            if(from.x-to.x == 1) return 1;
            if(from.x == 6 && from.x - to.x == 2) return 1;
        }
    }
    return 0;
}

bool chess_piece::rook_range(spot from, spot to, int board[8][8]){
    if(from.y == to.y){ //up and down
        if(from.x > to.x){ //up
            for(int i = from.x-1; i > to.x; i--){
                if(board[i][from.y] != -1) return 0;
            }
            return 1;
        }
        else if(from.x < to.x){ //down
            for(int i = from.x+1; i < to.x; i++){
                if(board[i][from.y] != -1) return 0;
            }
            return 1;
        }
    }
    else if(from.x == to.x){ //left and right
        if(from.y < to.y){ //right
            for(int i = from.y+1; i < to.y; i++){
                if(board[from.x][i] != -1) return 0;
            }
            return 1;
        }
        else if(from.y > to.y){ //left
            for(int i = from.y-1; i > to.y; i--){
                if(board[from.x][i] != -1) return 0;
            }
            return 1;
        }
    }
    return 0;
}
bool chess_piece::bishop_range(spot from, spot to, int board[8][8]){
    if(abs(from.x-to.x) != abs(from.y-to.y)) return 0;
    if(from.x > to.x && from.y < to.y){ //dia_up_right
        for(int i = from.x-1, j = from.y+1; i > to.x && j < to.y; i--, j++){
            if(board[i][j] != -1) return 0;
        }
        return 1;
    }
    else if(from.x > to.x && from.y > to.y){ //dia_up_left
        for(int i = from.x-1, j = from.y-1; i > to.x && j > to.y; i--, j--){
            if(board[i][j] != -1) return 0;
        }
        return 1;
    }
    else if(from.x < to.x && from.y < to.y){ //dia_down_right
        for(int i = from.x+1, j = from.y+1; i < to.x && j < to.y; i++, j++){
            if(board[i][j] != -1) return 0;
        }
        return 1;
    }
    else if(from.x < to.x && from.y > to.y){ //dia_down_left
        for(int i = from.x+1, j = from.y-1; i < to.x && j > to.y; i++, j--){
            if(board[i][j] != -1) return 0;
        }
        return 1;
    }
    return 0;
}
bool chess_piece::knight_range(spot from, spot to){
    int dx[] = {-1, -1, 1, 1, 2, 2, -2, -2};
    int dy[] = {2, -2, -2, 2, 1, -1, 1, -1};
    for(int i = 0; i < 8; i++){
        if(from.x+dx[i] == to.x && from.y+dy[i] == to.y) return 1;
    }
    return 0;
}
bool chess_piece::king_range(spot from, spot to){
    int dx[] = {-1, 1, 0, 0, 1, 1, -1, -1};
    int dy[] = {0, 0, 1, -1, 1, -1, 1, -1};
    for(int i = 0 ;i < 8; i++){
        if(from.x+dx[i] == to.x && from.y+dy[i] == to.y){
            return 1;
        }
    }
    return 0;
}
