#include "surfaces.h"


const int SURFACE_WIDTH = 9;
// NUM_SURFACES = std::pow(9, SURFACE_WIDTH - 1); //Maybe long long?
const std::vector<std::vector<std::vector<int>>> orients{
  { // T
    {3, -1, 1, 1, 2, 1},
    {2, 1, 3, 1},
    {3, 0, 0, 1, 2, 1},
    {2, -1, 1, 3}
  },
  { // J
    {3, 0, -1, 1, 1, 2},
    {2, 2, 3, 1},
    {3, 0, 0, 2, 1, 1},
    {2, 0, 1, 3}
  },
  { // Z
    {3, -1, 0, 1, 2, 1},
    {2, 1, 2, 2}
  },
  { // O
    {2, 0, 2, 2}
  },
  { // S
    {3, 0, 1, 1, 2, 1},
    {2, -1, 2, 2}
  },
  { // L
    {3, 1, 0, 2, 1, 1},
    {2, 0, 3, 1},
    {3, 0, 0, 1, 1, 2},
    {2, -2, 1, 3}
  },
  { // I
    {4, 0, 0, 0, 1, 1, 1, 1},
    {1, 4}
  }
};

int * intToSurface(int surface){
  // static int bumps[SURFACE_WIDTH - 1];
  int * bumps = new int[SURFACE_WIDTH - 1];
  for(int i = SURFACE_WIDTH - 2; i >= 0; i--){
    bumps[i] = (surface % 9) - 4;
    surface /= 9;
  }
  return bumps;
}

int surfaceToInt(int * surface){
  int res = 0;
  for(int i = 0; i < SURFACE_WIDTH - 1; i++){
    res *= 9;
    res += surface[i] + 4;
  }
  return res;
}

std::string printSurface(int * surface){
  int minOff = 0;
  int maxOff = 0;
  int curOff = 0;
  for(int i = 0; i < SURFACE_WIDTH - 1; i++){
    curOff += surface[i];
    minOff = std::min(minOff, curOff);
    maxOff = std::max(maxOff, curOff);
  }
  char block = 'o';
  char wall = '|';
  char floor = '-';
  int stackHeight = maxOff - minOff;
  char surfaceViz[stackHeight + 3][SURFACE_WIDTH + 4];
  curOff = -1 * minOff;
  for(int i = 0; i < SURFACE_WIDTH + 4; i++){
    surfaceViz[0][i] = floor;
    surfaceViz[1][i] = ' ';
    surfaceViz[stackHeight + 2][i] = floor;
  }
  for(int i = 0; i < stackHeight + 2; i++){
    if(i > 0){
      surfaceViz[i][0] = wall;
      surfaceViz[i][SURFACE_WIDTH + 2] = wall;

    }
    surfaceViz[i][SURFACE_WIDTH + 1] = ' ';
    surfaceViz[i][SURFACE_WIDTH + 3] = '\n';
  }
  surfaceViz[0][SURFACE_WIDTH + 1] = floor;

  for(int j = 0; j < SURFACE_WIDTH; j++){
    for(int i = 0; i < stackHeight; i++){
      surfaceViz[i + 2][j + 1] = (i + curOff < stackHeight) ? ' ' : block;
    }
    curOff += surface[j];
  }

  return std::string(&surfaceViz[0][0], &surfaceViz[stackHeight + 2][SURFACE_WIDTH + 3]);
}

bool validSurface(int * surface){
  for(int i = 0; i < SURFACE_WIDTH - 1; i++){
    if(surface[i] < -4 || surface[i] > 4){
      return false;
    }
  }
  return true;
}

int * createRandSurface(){
  srand(time(NULL));
  int mod = pow(9, SURFACE_WIDTH - 1);
  return intToSurface(rand() % mod);
}

// return nullptr if it isn't clean.
int * addPieceToSurface(int * surface, int piece, int orientation, int position){
  // Check if piece is too far right.
  const std::vector<int>& pieceData = orients[piece][orientation];
  int width = pieceData[0];
  if(position + width > SURFACE_WIDTH){
    return nullptr;
  }
  // Check if surfaces align
  for(int i = 0; i < width - 1; i++){
    if(pieceData[i + 1] != surface[position + i]){
      return nullptr;
    }
  }
  // static int newSurface[SURFACE_WIDTH - 1];
  int * newSurface = new int[SURFACE_WIDTH - 1];
  memcpy(newSurface, surface, sizeof(int) * (SURFACE_WIDTH - 1));

  for(int i = 1; i < width; i++){
    newSurface[i + position - 1] += pieceData[width + i] - pieceData[width + i - 1];
  }
  if(position > 0){
    newSurface[position - 1] += pieceData[width];
  }
  if(width + position < SURFACE_WIDTH){
    newSurface[width + position - 1] -= pieceData[2 * width - 1];
  }
  return newSurface;
}
