#pragma once
#include "gameParts.hpp"
#include "search.hpp"
#include <cstring>
#include <utility>
#include <random>

class GameState{
private:
  friend class Controller;
protected:
  int seed;
  Board board;
  Piece piece;
  Piece next;
  Search* pSearch;

public:
  int score;
  GameState(int nSeed, Search& nSearch);
  ~GameState();
  int findPlace();
  void overlap();
  void setPiece(const Piece& nPiece);
  void place();
  void place(const Piece& nPiece);
  void update();
  void search(std::vector<Piece>& places);
  std::string toString();
};
