#include "controller.hpp"
#include <iostream>

Controller::Controller(){
  int fd = open(FILE_NAME, O_RDWR|O_CREAT, 0666);
  if(fd == -1){
    std::cout << "Error getting file\n";

  }
  ranks = reinterpret_cast<float*>(mmap(nullptr, RANK_SIZE,
      PROT_READ|PROT_WRITE,
      MAP_FILE|MAP_SHARED,
      fd, 0));
}

Controller::~Controller(){
  // mummap the ranks
  munmap(ranks, RANK_SIZE);
  close(fd);
}

// Signals impossible move with the impossible default piece.
Piece Controller::move(GameState& state){
  std::vector<Piece> places;
  places.reserve(64);
  state.search(places);
  float maxValue = 0.0f;
  Piece pBest;
  int* bSurface = new int[WIDTH - 2];
  for(auto& p : places){
    // Add piece to board
    state.board.place(p);
    // Check if holes are formed (see if the cell directly below any piece cell is empty).
    std::array<std::pair<int, int>, 4> pOffs = p.getOffs();
    bool bad = false;
    for(auto& off : pOffs){
      if(off.first + p.getY() + 1 >= 0 && off.first + p.getY() + 1 < HEIGHT && !state.board.getCell(off.second + p.getX(), off.first + p.getY() + 1)){
        bad = true;
        break;
      }
    }
    if(bad){
      state.board.remove(p);
      continue;
    }

    state.board.toSurface(bSurface);
    for(int i = 0; i < WIDTH - 2; i++){
      if(bSurface[i] < -4 || bSurface[i] > 4){
        bad = true;
        break;
      }
      // if(bSurface[i] < -4) bSurface[i] = -4;
      // if(bSurface[i] > 4) bSurface[i] = 4;
    }
    if(bad){
      // std::cout << "Holes?\n";
      state.board.remove(p);
      continue;
    }
    // Figure out its value


    int bSurfaceNum = surfaceToInt(bSurface);
    float newValue = ranks[bSurfaceNum];
    // If better than max, set pBest accordingly.
    if(newValue > maxValue){
      maxValue = newValue;
      pBest = p;
    }
    // Don't forget to remove
    state.board.remove(p);
  }
  delete[] bSurface;
  return pBest;
}
