#include "surfaces.h"
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <math.h> // For stdev
// #include <time.h> // For random surface
#include <vector>
#include <chrono>
// #include <cstring>


const unsigned int NUM_SURFACES = std::pow(9, SURFACE_WIDTH - 1); //Maybe long long?
const unsigned int RANK_SIZE = NUM_SURFACES * sizeof(float);
const char * FILE_NAME = "ranks";
const float initRank = 1.0e9;
const int numThreads = 4;

/*
First index is the piece i,
numOrients[i] is the size of this array. For each orientation, first elem x indicates
number of columns the oriented piece spans, next x - 1 is bottom surface, then final
x is each column height.
*/

float * ranks[2];

struct Stats{
  int size;
  float avgSquErr;
  float avgErr;
  float maxErr;
  int bestSurface;
  float bestRank;
  float avgRank;

  void update(){

  }

  Stats merge(Stats otherStats){

  }

};

// int * intToSurface(int surface);
// int surfaceToInt(int * surface);
// std::string printSurface(int * surface);
// bool validSurface(int * surface);
// int * createRandSurface();
// int * addPieceToSurface(int * surface, int piece, int orientation, int position);

void setRanks(int start, int end);
float rank(int iteration, int stack);
float rankPiece(int iteration, int stack, int piece);
float rankOrientation(int iteration, int stack, int piece, int orientation);

void test(){
  int * testSurface1 = createRandSurface();
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
  int fd = open(FILE_NAME, O_RDWR|O_CREAT, 0666);
  if(fd == -1){
    std::cout << "Error getting file\n";
    return 1;
  }
  struct stat stat_buf;
  fstat(fd, &stat_buf); //Not cross platform
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
    delete[] ranks[0];
    ranks[0] = reinterpret_cast<float*>(mmap(nullptr, RANK_SIZE,
      PROT_READ|PROT_WRITE,
      MAP_FILE|MAP_SHARED,
      fd, 0));
    if(ranks[0] == MAP_FAILED){
      std::cout << "Error\n";
      return 1;
    }
    mapped = true;
  }

  std::cout << "Beginning the ranking process...\n";

  auto trueStart = std::chrono::steady_clock::now();
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
    auto start = std::chrono::steady_clock::now();
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
    auto end = std::chrono::steady_clock::now();
    avgErr /= (float) NUM_SURFACES;
    avgSquErr /= (float) NUM_SURFACES;
    avgRank /= (float) NUM_SURFACES;
    float stdErr = sqrt(avgSquErr - avgErr * avgErr);
    // Want to print the max err and the avg error (maybe stdev also?). Put this in log file
    std::cout << "Iteration " << (i + 1) << " completed\n";
    std::cout << "Duration: " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << "s\n";
    std::cout << "Average rank: " << avgRank << "\n";
    std::cout << "Max error: " << maxErr << "\n";
    std::cout << "Average error: " << avgErr << "\n";
    std::cout << "Standard deviation: " << stdErr << "\n\n";
    std::cout << "Best surface:\n";
    std::cout << printSurface(intToSurface(bestSurface)) << "\n" << std::endl;

    start = end;
  }


  auto trueEnd = std::chrono::steady_clock::now();
  std::cout << "Total duration: " <<
    std::chrono::duration_cast<std::chrono::seconds>(trueEnd - trueStart).count() << "s\n\n";

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

  if(mapped) munmap(ranks[0], RANK_SIZE);

  // Write final ranks to file if necessary
  if(iters % 2 == 1 || !mapped){
      write(fd, ranks[iters % 2], RANK_SIZE);
  }
  close(fd);
  return 0;
}


void setRanks(int start, int end){
  for(int i = start; i < end; i++){

  }
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
    delete[] newSurface;
  }
  delete[] surface;

  return maxRank;
}
