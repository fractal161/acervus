#pragma once
#include "../rank/surfaces.hpp"
#include "gameParts.hpp"
#include <vector>
#include <array>
#include <utility>



// Every search method must return a list of all possible coordinates/piece orientation


class Search{
private:

public:
  virtual void search(std::vector<Piece>& places, const Board& board, Piece& piece) = 0;
  virtual void clean() = 0;
};

class BasicDfs: public Search{
private:
  bool visited[WIDTH][HEIGHT][4];
public:
  BasicDfs();
  void search(std::vector<Piece>& places, const Board& board, Piece& piece);
  void clean();
};
