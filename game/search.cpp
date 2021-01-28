#include "search.hpp"
#include <iostream>
/*
STRUCTURE
Most basic search algorithm is going left to right and hard dropping.
Will need to physically shift piece to make sure overlap doesn't happen.
Hopefully polevaluting doesn't matter because that'd be ridiculous.
Eh might just do BFS.

Next is BFS. Each state includes positional information, along with gravity/DAS maybe.

Assume moves are processed in order shift, rotate, drop.

For each iteration of the BFS, store all possible child states.
(Probably better with NES tetris with known frame timings)

Store all valid resting positions somewhere else.

This can be precomputed down to the highest column, which could be advantageous.
*/

BasicDfs::BasicDfs(){
  for(int i = 0; i < WIDTH; i++){
    for(int j = 0; j < HEIGHT; j++){
      for(int k = 0; k < 4; k++){
        visited[i][j][k] = false;
      }
    }
  }
}

void BasicDfs::search(std::vector<Piece>& places, const Board& board, Piece& piece){
  // Check if position is valid, and if so mark as visited.
  int x = piece.getX();
  int y = piece.getY();
  int orient = piece.getOrient();
  if(board.overlap(piece) || visited[x][y][orient]){
    return;
  }
  visited[x][y][orient] = true;

  // Check to see if surface can accommodate
  Piece nPiece = piece;
  while(!board.overlap(nPiece)){
    nPiece.drop(1);
  }
  nPiece.drop(-1);
  places.push_back(nPiece);

  // Given position, search all other rotation, then left+right (not composite)
  piece.shift(1);
  search(places, board, piece);
  piece.shift(-2);
  search(places, board, piece);
  piece.shift(1);
  //Rotate a ton.
  piece.rotate(1);
  search(places, board, piece);
  piece.rotate(2);
  search(places, board, piece);
  piece.rotate(1);
}

void BasicDfs::clean(){
  // Reset internal values
  for(int i = 0; i < WIDTH; i++){
    for(int j = 0; j < HEIGHT; j++){
      for(int k = 0; k < 4; k++){
        visited[i][j][k] = false;
      }
    }
  }
}
