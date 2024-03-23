1- Wanna play?
    1-Invite a friend using his IP and port number
    (shoud be open and the friend accept your invitation)
    2-Listen to a friend 
    (bind socket to an interface with IP, specify a port number)
    (Who listens will play first with white color)
    3-History (Not implemented yet)
    (view past mathches results)
        option: play the match
----------------------------
2-Init pieces and board
    There are 2x16 pieces on the board we init there types and locations
---------------------------
3-Check status of the game
    In the start of each play check the king status:
    1-other player lose?
        I win and break
    2-Other player draw?
        I draw and break
    3-King's spot is not safe?
        can move?    ->  checkmate
        can't move? 
            try for each of you pieces every available move
            and check if the king will be safe or not.
            found a move ?     -> checkmate
            no such move ?     -> lose, send lose and break
    4-King's spot is safe?
        can move?    ->  good
        can't move?
            no other pieces can move ?  -> draw (stalemate), send draw and break
---------------------------
4- Is it my turn?
    YES?   do(A)
    NO?    do (B)

(A) Make a move
    How to check a move if it's valid?
        1-Not out of bound of the chess board.
        2-Not an empty spot.
        3-Not moving enemy's piece
        5-Castling
            valid ? 
                do castling and update board
            not valid ? 
                print "not valid"
        6-Not attacking allied piece.
        7-Valid piece range

    valid ? 
        try to make the move
    is king safe ?
        No ? 
            roll back the move and print "not valid"
            goto(4)
        Yes ? 
            update the board with the valid move, send the move and change the turn.
            goto(4)

(B) recive other player's reply 
    recive move
    move is -1 ? 
        other player lose (I win)
    move is -2
        other player draw (I draw)
    else
        update the board and change the turn
        goto(4)
        
--------------------------
History feature (not implemented yet)
5-At end of every game (destructor)
    save the match
        print date, guest name and moves to a file


How to contribute:
1- This game doesn't detect all draw cases (just stalemate)
        Any PR handling a game draw rule will be reviewed and accepted
2- Any special moves (rather than pawn promotion and castling)
    -Any Chess rules not coverd-
        inclue the rule reference in your PR

Future features (Not accepting PRs):
1-History
2-GUI
3-chat (Threading)
4-Timer (Threading)
5-Chess agent (API)
6-Chess engine (AI)
