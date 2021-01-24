#include "gameState.hpp"
#include <iostream>

GameState::GameState(int nSeed, Search* nSearch){
  pSearch = nSearch;
  seed = nSeed;
  score = 0;
  // srand(nSeed);
  piece = Piece(rand() % 7);
  next = Piece(rand() % 7);
}

// GameState::~GameState(){}

void GameState::setPiece(const Piece& nPiece){
  piece = nPiece;
}

void GameState::place(){
  board.place(piece);
  board.clearRows();
}


void GameState::search(std::vector<Piece>& places){
  // std::cout << "Before search\n";
  // std::cout << this->toString() << "\n";
  pSearch->search(places, board, piece);
  // std::cout << "After search\n";
  // std::cout << this->toString() << "\n";
  pSearch->clean();
}

void GameState::update(){
  piece = next;
  next = Piece(rand() % 7);
  score++;
}

std::string GameState::toString(){
  std::string game =
  "  0123456789         \n"
  " ------------ -------\n" // 22 characters wide
  "0|..........| |SCORE|\n"
  "1|..........| |00000|\n"
  "2|..........| |     |\n"
  "3|..........| -------\n"
  "4|..........|        \n"
  "5|..........| -------\n"
  "6|..........| |NEXT |\n"
  "7|..........| |     |\n"
  "8|..........| |     |\n"
  "9|..........| |     |\n"
  "0|..........| |     |\n"
  "1|..........| |     |\n"
  "2|..........| -------\n"
  "3|..........|        \n"
  "4|..........| -------\n"
  "5|..........| |LEVEL|\n"
  "6|..........| |     |\n"
  "7|..........| |     |\n"
  "8|..........| -------\n"
  "9|..........|        \n"
  " ------------        \n"
  "  0123456789";

  // Score
  int tScore = score;
  int sPos = 19;
  while(tScore > 0){
    game[22 * 3 + sPos] = '0' + (tScore % 10);
    sPos--;
    tScore /= 10;
  }

  // Board
  for(int i = 0; i < WIDTH; i++){
    for(int j = 0; j < HEIGHT; j++){
      if(board.getCell(i, j)){
        game[22 * (j + 2) + 2 + i] = 'X';
      }
    }
  }

  // Active piece
  std::array<std::pair<int, int>,4> pOffs = piece.getOffs();
  for(auto& off : pOffs){
    game[22 * (off.first + piece.getY() + 2) + 2 + off.second + piece.getX()] = '#';
  }

  // Next piece
  // std::array<std::pair<int, int>,4> nOffs = next.getOffs();
  // for(auto& off : nOffs){
  //   game[22 * (off.first + 9 + 2) + 2 + off.second + 15] = '#';
  // }
  return game;
}
