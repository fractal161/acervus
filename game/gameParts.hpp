#pragma once
#include "../rank/surfaces.hpp"
#include <array>
#include <vector>
#include <utility>


const int WIDTH = 10;
const int HEIGHT = 20;
const std::array<int, 7>nOrients = {4, 4, 2, 1, 2, 4, 2};
const std::vector<std::array<std::pair<int, int>, 4>> pCoords[7] = {
    {{{{1, 0}, {0, 0}, {0, 1}, {0, -1}}}, // T
     {{{1, 0}, {0, 0}, {0, 1}, {-1, 0}}},
     {{{0, -1}, {0, 0}, {0, 1}, {-1, 0}}},
     {{{1, 0}, {0, 0}, {-1, 0}, {0, -1}}}},
    {{{{0, -1}, {0, 0}, {0, 1}, {1, 1}}}, // J
     {{{-1, 0}, {-1, 1}, {0, 0}, {1, 0}}},
     {{{-1, -1}, {0, -1}, {0, 0}, {0, 1}}},
     {{{-1, 0}, {0, 0}, {1, -1}, {1, 0}}}},
    {{{{0, -1}, {0, 0}, {1, 0}, {1, 1}}}, // Z
     {{{-1, 1}, {0, 0}, {0, 1}, {1, 0}}}},
    {{{{0, -1}, {0, 0}, {1, -1}, {1, 0}}}}, // O
    {{{{0, 0}, {0, 1}, {1, -1}, {1, 0}}}, // S
     {{{-1, 0}, {0, 0}, {0, 1}, {1, 1}}}},
    {{{{0, -1}, {0, 0}, {0, 1}, {1, -1}}}, // L
     {{{-1, 0}, {0, 0}, {1, 0}, {1, 1}}},
     {{{-1, 1}, {0, -1}, {0, 0}, {0, 1}}},
     {{{-1, -1}, {-1, 0}, {0, 0}, {1, 0}}}},
    {{{{0, -2}, {0, -1}, {0, 0}, {0, 1}}}, // I
     {{{-2, 0}, {-1, 0}, {0, 0}, {1, 0}}}}};

class Piece{
private:
       // MIGHT HAVE TO REARRANGE THESE (he does like y then x or something)
  int type;
  // std::array<int,3> pos;
  int x;
  int y;
  int orient;

public:
  Piece();
  Piece(int nType);
  ~Piece();
  int numOrients() const;
  const std::array<std::pair<int, int>,4>& getOffs() const;
  int getX () const;
  int getY() const;
  int getOrient() const;
  void shift(int off);
  void rotate(int amt);
  void drop(int off);
  void setPos(int x, int y, int orient);
  void operator =(const Piece& p);
};

class Board{
private:
  std::array<std::array<bool, WIDTH>, HEIGHT> cells;
  friend class Search;
public:
  Board();
  bool getCell(int x, int y) const;
  void setCell(int x, int y);
  bool overlap(const Piece& piece) const;
  void place(const Piece& piece); //, position pos);
  void remove(const Piece& piece);
  //Bitwise operators to make this fast.
  int clearRows();//Bottom row mask is 2^0 + 2^20 + ..., bitshift for the rest
  void toSurface(int* buffer);
};
