#pragma once
enum color {white, black};
enum type {pawn, rook, knight, bishop, king, queen};

struct spot{
    int x, y;
};

class chess_piece {
    protected:
        color clr;
    public:
        static int killed[2][6];
        chess_piece(color c) : clr(c) { }
        virtual ~chess_piece() { }
        color get_color();
        virtual void print_piece() = 0;
        virtual type get_type() = 0;
        virtual bool can_reach(spot, spot) = 0;
};

class Pawn : public chess_piece {
    public:
        Pawn(color clr) : chess_piece(clr){ }
        void print_piece();
        type get_type();
        bool can_reach(spot, spot);
};
class Rook : public chess_piece {
    public:
        Rook(color clr) : chess_piece(clr) { }
        void print_piece();
        type get_type();
        bool can_reach(spot, spot);
};
class Knight : public chess_piece {
    public:
        Knight(color clr) : chess_piece(clr) { }
        void print_piece();
        type get_type();
        bool can_reach(spot, spot);
};
class Bishop : public chess_piece {
    public:
        Bishop(color clr) : chess_piece(clr) { }
        void print_piece();
        type get_type();
        bool can_reach(spot, spot);
};
class King : public chess_piece {
    public:
        King(color clr) : chess_piece(clr) { }
        void print_piece();
        type get_type();
        bool can_reach(spot, spot);
};
class Queen : public chess_piece {
    public:
        Queen(color clr) : chess_piece(clr) { }
        void print_piece();
        type get_type();
        bool can_reach(spot, spot);
};
