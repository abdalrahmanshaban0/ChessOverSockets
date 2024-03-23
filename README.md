# ChessOverSockets
### 1. Wanna play?<br>
    1-Invite a friend using his IP and port number
    (shoud be open and the friend accept your invitation)
    2-Listen to a friend
    (bind socket to an interface with IP, specify a port number)
    (Who listens will play first with white color)
    3-History (Not implemented yet)
    (view past mathches results)
    option: play the match
### 2. Init pieces and board<br>
    There are 2x16 pieces on the board we init there types and locations
### 3. Check status of the game<br>
In the start of each play check the king status:<br>
- other player lose?<br>
- I win and break<br>
- Other player draw?<br>
    - I draw and break<br>
- King's spot is not safe?<br>
    - can move?    ->  checkmate<br>
    - can't move? <br>
        -try for each of you pieces every available move<br>
        -and check if the king will be safe or not.<br>
    - found a move ?     -> checkmate<br>
    - no such move ?     -> lose, send lose and break<br>
### 4-King's spot is safe?<br>
    can move?    ->  good<br>
    can't move?<br>
        no other pieces can move ?  -> draw (stalemate), send draw and break<br>
4- Is it my turn?<br>
    YES?   do(A)<br>
    NO?    do (B)<br>

(A) Make a move<br>
How to check a move if it's valid?<br>
    1-Not out of bound of the chess board.<br>
    2-Not an empty spot.<br>
    3-Not moving enemy's piece<br>
    4-Castling<br>
        valid ? <br>
            do castling and update board<br>
        not valid ? <br>
            print "not valid"<br>
    5-Not attacking allied piece.<br>
    6-Valid piece range<br>

valid ? <br>
try to make the move<br>
is king safe ?<br>
No ? <br>
roll back the move and print "not valid"<br>
goto(4)<br>
Yes ? <br>
    update the board with the valid move, send the move and change the turn.<br>
goto(4)<br>
(B) recive other player's reply<br> 

recive move<br> 
    move is -1 ? <br> 
other player lose (I win)<br> 
    move is -2<br> 
other player draw (I draw)<br> 
    else<br> 
    update the board and change the turn<br> 
    goto(4)<br> 

History feature (not implemented yet)<br>
5-At end of every game (destructor)<br>
save the match<br>
print date, guest name and moves to a file<br>

How to contribute:<br>
1- This game doesn't detect all draw cases (just stalemate)<br>
    Any PR handling a game draw rule will be reviewed and accepted<br>
2- Any special moves (rather than pawn promotion and castling)<br>
    -Any Chess rules not coverd-<br>
    inclue the rule reference in your PR<br>

Future features (Not accepting PRs):<br>
    1-History<br>
    2-GUI<br>
    3-chat (Threading)<br>
    4-Timer (Threading)<br>
    5-Chess agent (API)<br>
    6-Chess engine (AI)<br>
