# ChessOverSockets
![image](https://github.com/abdalrahmanshaban0/ChessOverSockets/assets/126330281/d6a99fb2-f9f4-4dd4-93e5-233ef582f59a)

![image](https://github.com/abdalrahmanshaban0/ChessOverSockets/assets/126330281/73198e7f-a210-4168-a4a3-c6759330d747)

### 1. Wanna play?<br>
1. Invite a friend using his IP and port number<br>
   (shoud be open and the friend accept your invitation)
2. Listen to a friend
   (bind socket to an interface with IP, specify a port number)<br>
   (Who listens will play first with white color)
3. History (Not implemented yet) <br>
   (view past mathches results)<br>
   option: play the match
### 2. Init pieces and board<br>
There are 2x16 pieces on the board, init there types and locations
### 3. Check status of the game<br>
In the start of each play check the king status:<br>
- other player lose?<br>
    - I win and break<br>
- Other player draw?<br>
    - I draw and break<br>

- try for each of your pieces every available move<br>
  and check if the king will be safe or not.<br>
     - King is not safe?<br>
          - A move found to disable check ?<br>
             - YES ?  -> checkmate<br>
             - NO ?   -> send lose and break<br>
     - king is safe?<br>
          - Any valid move found?<br>
             - YES ? ->  good<br>
             - NO ?  -> send draw (stalemate) and break<br>

### 4. Is it my turn?<br>
- YES?   do(A)<br>
- NO?    do (B)<br>

(A) Make a move<br>
How to check a move if it's valid?<br>
   1. Not out of bound of the chess board.<br>
   2. Not an empty spot.<br>
   3. Not moving enemy's piece<br>
   4. Castling<br>
      - valid ? <br>
         - do castling and update board<br>
      - not valid ? <br>
         - print "not valid"<br>
   5. Not attacking allied piece.<br>
   6. Valid piece range<br>

valid ? <br>
   - try to make the move<br>
   - is king safe ?<br>
      - No ? <br>
         - roll back the move and print "not valid"<br>
         - goto(4)<br>
      - Yes ? <br>
         - update the board with the valid move, send the move and change the turn.<br>
         - goto(3)<br>

(B) recive other player's reply<br> 
recive move<br> 
   - move is -1 ? <br> 
      - other player lose (I win)<br> 
   - move is -2 ? <br> 
      - other player draw (I draw)<br> 
   - else<br> 
      - update the board and change the turn<br> 
      - goto(3)<br> 

History feature (not implemented yet)<br>

### How to contribute:<br>
1. This game doesn't detect all draw cases (just stalemate)<br>
    Any PR handling a game draw case will be reviewed and accepted<br>
2. Any special moves (rather than pawn promotion and castling)<br>
    -Any Chess rules not coverd-<br>
    inclue the rule reference in your PR<br>

### Possible comming features (Not accepting PRs):<br>
 - History<br>
 - GUI<br>
 - chat (Threading)<br>
 - Timer (Threading)<br>
 - Chess agent (API)<br>
 - Chess engine (AI)<br>
