
# Bot for Othello
Bot uses Principle Variation Search with Iterative Deepening to find the best move. Board position is estimated as a sum of mobility and total stone value. 
Mobility is a difference between number of moves available to bot and number of moves available to it's opponent. Moves in corners are counted twice as possibility of taking a corner is important. X and C squares are not counted at all as it is generally a bad idea to take these squares. 
Total stone value is calculated as a sum of stone values. Corner stone value is 10, X stone value is -5 and C stone value is -2. Opponent's stone values are taken with negative signs.
Introduction to basic strategies and principles of othello could be found here, for example. https://www.ultraboardgames.com/othello/strategy.php
# How to play with the bot

Compile the program using

     mkdir build && cd build
     cmake ../
     make
 
 Then run with 
 
    ./othello $color $time
$color specifies which color you want to get and it could be either 'black' or 'white'. $time is a maximal thinking time for the bot in milliseconds.
example:

    ./othello black 1000

To make a move enter coordinates of a position where you want to place a stone. First type column, then row.
For example you want to place a stone at 5th row 2nd column:

     abcdefgh
    1........1
    2........2
    3...B....3
    4...BB.B.4
    5.*WWBWB.5
    6....WWBW6
    7....WWB.7
    8........8
     abcdefgh
 Then type
 
    b5 