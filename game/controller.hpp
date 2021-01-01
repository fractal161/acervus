#pragma once
#include "gameState.hpp"
#include "gameParts.hpp"
#include "../rank/surfaces.hpp"
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>

class Controller{
private:
  const unsigned int NUM_SURFACES = std::pow(9, SURFACE_WIDTH - 1); //Maybe long long?
  const unsigned int RANK_SIZE = NUM_SURFACES * sizeof(float);
  const char* FILE_NAME = "../data/ranksEv9";
  int fd;
  float* ranks;
public:
  Controller();
  ~Controller();
  Piece move(GameState& state);
};
