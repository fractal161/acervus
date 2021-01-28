#pragma once
#include <cstdint>
#include "../rank/surfaces.hpp"
#include "constants.hpp"



class Piece{
private:

  int type;
  int x;
  int y;
  int orient;

public:
  Piece();
  Piece(int nType);
  ~Piece();
  int numOrients() const;
  const std::array<std::pair<int, int>,4>& getOffs() const;
  int getX() const;
  int getY() const;
  int getType() const;
  int getOrient() const;
  void shift(int off);
  void rotate(int amt);
  void drop(int off);
  void setPos(int x, int y, int orient);
  void operator =(const Piece& p);
  bool inBounds() const;
  uint64_t getBits() const;
};

class Board{
private:
  // bits 0-9 are filled with 1s, bits 10-209 are normal, bits 210-229 are 0s.
  std::array<uint64_t, 4> cells;
  std::array<uint64_t*, 20> rows;
  int maxCol;
  int maxColTmp;
public:
  Board();
  bool getCell(int x, int y) const; // not bad yet?
  void setCell(int x, int y, bool value); // not bad yet?
  bool overlap(const Piece& piece) const; // BAD
  void place(const Piece& piece);
  void remove(const Piece& piece);
  //Bitwise operators to make this fast.
  int clearRows();
  void toSurface(int* buffer); // BAD
  bool isEmpty() const;
};

/*
Need to figure out coordinate system. Here's a list of all functions that directly depend on it:
GameState::toString()
Controller::move()
*/
