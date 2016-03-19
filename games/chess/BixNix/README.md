# BixNix Chess Engine

This engine features the following:

### Negamax with Alpha Beta Pruning
- It's just Min-Max with care taken in state evaluation

### Time Limiting
- After each depth is completed, makes an estimate of time needed to complete
the next depth. Does not start next depth if estimated completion time
exceeds predetermined limit.
- Estimate assumes exponential growth.

### Transposition Table
- Store Score, Depth, Move, Node Type (exact, upper bound, lower bound)
- Sized via the frown test to achieve an acceptable collision rate

### Zobrist Hashing
- Random seed unoptimized

### Magic Bitboards
- Offsets generated with a greedy algorithm

### State Evaluation
- Material Evaluation with standard values
- Piece Square State Evaluation
- Draws valued 1 centipawn higher than being checkmated
- Stalemate detection
- 100 ply capture / pawn move detection
- Threefold Board State Repetition detection

### Move Generation
- Metadata cached into 32 bit integer, accelerating move application
- Flag available for "Best Possible Move", triggering immediate move send

### Move Ordering
- PV Move first, if valid
- Move from Transposition Table next, if available
- Then Castling Moves
- Then Captures, in order of Least Valuable Attacker / Most Valuable Target
- Finally quiet moves, ordered by Piece Square differential

### Late Move Reductions
Reduce depth of search of quiet moves late in the move order.

### Opening Book
http://www.chess2u.com/t7448-komodo-variety-opening-book-komodo-polyglot-book
- Reads Polyglot format
- Fitness Proportional move selection from entries in book
- Competing with Komodo Variety book

### Logging
- Output each time alpha increases at root node
- Output a variety of statistics at end of each game

### Threading
- Searcher runs on seperate thread from Time Limiter
- Thread synchronization via Rendezvous construct from Plan 9

### Not Yet Implemented
- Insufficient Material detection
- Lazy inCheck filtering
- Lazy move generation
- Board::isValidMove(pvMove) to check it prior to move generation
- Endgame tablebase

### Implemented Then Discarded
- History Table, didn't help
- Quiescent Search, underperformed
