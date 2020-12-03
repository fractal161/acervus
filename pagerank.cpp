#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <math.h> // For stdev
#include <time.h> // For random surface
#include <vector>
#include <cstring>

const int SURFACE_WIDTH = 9;
const unsigned int NUM_SURFACES = std::pow(9, SURFACE_WIDTH - 1); //Maybe long long?
const unsigned int RANK_SIZE = NUM_SURFACES * sizeof(float);
const char * FILE_NAME = "ranks";
const float initRank = 1.0e9;
// TJZOSLI
// const int numOrients[] = {4, 4, 2, 1, 2, 4, 2};
/*
First index is the piece i,
numOrients[i] is the size of this array. For each orientation, first elem x indicates
number of columns the oriented piece spans, next x - 1 is bottom surface, then final
x is each column height.
*/
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

float * ranks[2];

int * intToSurface(int surface){
  static int bumps[SURFACE_WIDTH - 1];
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

int * createSurface(){
  srand(time(NULL));
  int mod = pow(9, SURFACE_WIDTH - 1);
  return intToSurface(rand() % mod);
}

// return nullptr if it isn't clean.
int * addPieceToSurface(int * surface, int piece, int orientation, int position){
  // Check if piece is too far right.
  std::vector<int>pieceData = orients[piece][orientation];
  int width = pieceData[0];
  if(position + width > SURFACE_WIDTH){ // COULD BE BUGGED.
    return nullptr;
  }
  // Check if surfaces align
  for(int i = 0; i < width - 1; i++){
    if(pieceData[i + 1] != surface[position + i]){
      return nullptr;
    }
  }
  static int newSurface[SURFACE_WIDTH - 1];
  memcpy(newSurface, surface, sizeof(int) * (SURFACE_WIDTH - 1));

  for(int i = 1; i < width; i++){
    newSurface[i + position - 1] += pieceData[width + i];
    newSurface[i + position - 1] -= pieceData[width + i - 1];
  }
  if(position > 0){
    newSurface[position - 1] += pieceData[width];
  }
  if(width + position < SURFACE_WIDTH){
    newSurface[width + position - 1] -= pieceData[2 * width - 1];
  }
  return newSurface;
}

float rank(int iteration, int stack);
float rankPiece(int iteration, int stack, int piece);
float rankOrientation(int iteration, int stack, int piece, int orientation);

void test(){
  int * testSurface1 = createSurface();
  for(int i = 0; i < 7; i++){
    for(int j = 0; j < orients[i].size(); j++){
      for(int k = 0; k < SURFACE_WIDTH; k++){
        std::cout << "test " << i << " " << j << " " << k << "\n";
        int * testSurface2 = addPieceToSurface(testSurface1, i, j, k);
        if(testSurface2 != nullptr){
          std::cout << printSurface(testSurface1) << "\n";
          std::cout << printSurface(testSurface2) << "\n";
          std::cout << "____________\n";
        }

      }
    }
  }
}

int main(int argc, char *argv[]){
  if(false){
    test();
    return 0;
  }
  int fd = open(FILE_NAME, O_RDWR|O_CREAT, 0664);
  if(fd == -1){
    std::cout << "Error getting file\n";
    return 1;
  }
  struct stat stat_buf;
  fstat(fd, &stat_buf);
  bool mapped = false;
  ranks[0] = new float[RANK_SIZE];
  ranks[1] = new float[RANK_SIZE];

  int iters;
  if(argc == 1){
    iters = 1;
  }
  else{
    iters = atoi(argv[1]);
  }

  if(stat_buf.st_size == 0){
    std::cout << "Initializing ranks\n";
    for(int i = 0; i < NUM_SURFACES; i++){
      ranks[0][i] = initRank;
    }
  }
  else if(stat_buf.st_size < RANK_SIZE){
    std::cout << "File is too small\n";
    return 1;
  }
  else{
    ranks[0] = reinterpret_cast<float*>(mmap(nullptr, RANK_SIZE,
      PROT_READ|PROT_WRITE|PROT_EXEC,
      MAP_FILE|MAP_SHARED,
      fd, 0));
    if(ranks[0] == MAP_FAILED){
      std::cout << "Error";
      return 1;
    }
    mapped = true;
  }
  // std::cout <<"test";
  // Finally the algorithm.
  std::cout << "Beginning the ranking process...\n";
  for(int i = 0; i < iters; i++){
    int row = (i + 1) % 2;
    int prev = i % 2;

    // Stats
    float avgSquErr = 0;
    float avgErr = 0;
    float maxErr = 0;
    int bestSurface = 0;
    float bestRank = 0;
    float avgRank = 0;

    for(int j = 0; j < NUM_SURFACES; j++){
      ranks[row][j] = rank(row, j);
      if(ranks[row][j] > bestRank){
        bestSurface = j;
        bestRank = ranks[row][j];
      }
      float err = abs(ranks[row][j] - ranks[prev][j]);
      maxErr = std::max(maxErr, err);
      avgErr += err;
      avgSquErr += err * err;
      avgRank += ranks[row][j];
    }

    avgErr /= (float) NUM_SURFACES;
    avgSquErr /= (float) NUM_SURFACES;
    avgRank /= (float) NUM_SURFACES;
    float stdErr = sqrt(avgSquErr - avgErr * avgErr);
    // Want to print the max err and the avg error (maybe stdev also?)
    std::cout << "Iteration " << (i + 1) << " completed\n";
    std::cout << "Average rank: " << avgRank << "\n";
    std::cout << "Max error: " << maxErr << "\n";
    std::cout << "Average error: " << avgErr << "\n";
    std::cout << "Standard deviation: " << stdErr << "\n\n";
    std::cout << "Best surface:\n";
    std::cout << printSurface(intToSurface(bestSurface)) << "\n\n";
  }
  // List n best surfaces.
  int n = 20;
  float lastMax = initRank;
  std::cout << "Top " << n << " surfaces\n\n";
  for(int i = 0; i < n; i++){
    float nextMax = 0.0;
    int nextStack = 0;
    for(int j = 0; j < NUM_SURFACES; j++){
      if(ranks[iters % 2][j] > nextMax && ranks[iters % 2][j] < lastMax){
        nextMax = ranks[iters % 2][j];
        nextStack = j;
      }
    }
    std::cout << printSurface(intToSurface(nextStack)) << "\n\n" << nextMax << "\n\n";
    lastMax = nextMax;
  }

  // Write final ranks to file
  write(fd, ranks[iters % 2], RANK_SIZE);

  close(fd);

  if(mapped) munmap(ranks[0], RANK_SIZE);

}

float rank(int iter, int stack){
  float avgRank = 0.0;
  for(int i = 0; i < 7; i++){
    avgRank += rankPiece(iter, stack, i);
  }
  return (avgRank / 7.0f);
}

float rankPiece(int iter, int stack, int piece){
  float maxRank = 0.0;
  for(int i = 0; i < orients[piece].size(); i++){
    maxRank = std::max(maxRank, rankOrientation(iter, stack, piece, i));
  }
  return maxRank;
}

// Use the rank values from the previous iteration
float rankOrientation(int iter, int stack, int piece, int orientation){
  int * surface = intToSurface(stack);
  int prev = (iter + 1) % 2;
  // Take the orientation, move it all across the surface and see if anything works.
  // If so, add it at that point and lookup the previous iteration's rank for that.
  // Return the max of all possibilities.
  float maxRank = 0.0;
  for(int i = 0; i < SURFACE_WIDTH - orients[piece][orientation][0] + 1; i++){
    int * newSurface = addPieceToSurface(surface, piece, orientation, i);
    if(newSurface != nullptr && validSurface(newSurface)){
      int newNum = surfaceToInt(newSurface);
      float newRank = ranks[prev][newNum];
      maxRank = std::max(maxRank, newRank);
    }
  }

  return maxRank;
  // return 1000.0;
}
