Implemented FEN functionality for board positions
Implemented castling
Implemented promotion
Implemented En Passant

Implemented negamax
Implemented Alpha Beta pruning
Negamax can run at least depth 3
Implemented Evaluate Board
AI Plays better than random

Previous assignment requirements working
Implemented removal of other piece movements causing illegal board states by placing their king into check with their movement
Implemented king not being able to move itself into check
Implemented that if a king is in check, only moves that take it out of check will be allowed
Implemented castling not being doable if any position along the way would put the king into check.
Implemented reporting a win or a draw based on the board state

Hardest part of this assignment was ensuring my king could not make a move that placed itself into check.  I did not think to update the king position if that is what was temporarily moved in the FilterIllegalMoves function.  The castling wasnt too bad, just do two checks instead of one.  Speed is obviously not a goal of mine here given the lack of bitboards, however given that they are optional and were a learning tool I chose to not implement them.  I got mated a few times and was unable to pinscer the opponents king properly.

My AI does well, i cant beat it even at depth 3 and with no illegal move restrictions (it'd cook me faster with them).  Efficiency is hard to measure however as I have too powerful of a desktop and I can just oonga boonga compute at it and it will just go fast.
Moves took anywhere from a second to 3 depending on the depth.

I realized very quickly into development, there is only 1 en passant square, so that simplified storage to being board wide
Pawm promotion is a simple texture swap
castling is a bit of hard coding, but given that the game logic is now complete, the next step for the next assignment is validity of moves (check preventing king or requiring king moves), but that is a task for next time.

as usual I build with
-- Selecting Windows SDK version 10.0.22621.0 to target Windows 10.0.19045.
and you can, via Command Prompt run
```sh
build && run
```
to run my program.

