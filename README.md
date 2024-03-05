Implemented FEN functionality for board positions
Implemented castling
Implemented promotion
Implemented En Passant

Implemented negamax
Implemented Alpha Beta pruning
Negamax can run at least depth 3
Implemented Evaluate Board
AI Plays better than random

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

