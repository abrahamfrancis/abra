# chess-engine
A simple chess engine written in C++ 17

## Build & run
* Build the engine
```sh
make
```
* Run it using
```sh
./engine --strategy minimax --color black
```
You can also pass a FEN to start playing from another position. For eg.
```sh
./engine --strategy minimax --color white --fen "rnbqkbnr/pppppppp/8/8/8/4P3/PPPP1PPP/RNBQKBNR b KQkq - 0 1"
```
