#include "surfaces.hpp"
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h> // For stdev
#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include <thread>

const unsigned int NUM_SURFACES = std::pow(9, SURFACE_WIDTH - 1);
const unsigned int RANK_SIZE = NUM_SURFACES * sizeof(float);
const char* FILE_NAME = "../data/ranksEv9";
const float initRank = 0.0f;
const int nThreads = 4;

float* ranks[2];

class RankStats{
private:
  int nSurfaces;
  int bestSurface;
  double bestRank;
  double maxErr;
  double totErr;
  double totSquErr;
  double totRank;

public:
  RankStats();
  void update(int row, int surface);
  void print();
  void merge(RankStats& otherStats);

};

void setRanks(int start, int end, int row, RankStats* stats);
float rank2(int iter, int stack);
void rankOrient2(int iter, int* surface, int piece, int orientation, int pos, float* pieceRanks);

int main(int argc, char* argv[]){
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
  else if(stat_buf.st_size != RANK_SIZE){
    std::cout << "File is the wrong size\n";
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

  using pSeconds = std::chrono::duration<double, std::ratio<1, 1>>;
  auto trueStart = std::chrono::steady_clock::now();
  for(int i = 0; i < iters; i++){
    int row = (i + 1) % 2;
    auto startTime = std::chrono::steady_clock::now();
    RankStats* stats = new RankStats[nThreads];
    std::thread rankThreads[nThreads];

    for(int i = 0; i < nThreads; i++){
      int start = (i * NUM_SURFACES) / nThreads;
      int end = ((i + 1) * NUM_SURFACES) / nThreads;
      rankThreads[i] = std::thread(setRanks, start, end, row, stats+i);
    }

    for(int i = 0; i < nThreads; i++){
      rankThreads[i].join();
    }
    for(int i = 1; i < nThreads; i++){
      stats[0].merge(stats[i]);
    }

    auto endTime = std::chrono::steady_clock::now();

    std::cout << "Iteration " << (i + 1) << " completed\n";
    std::cout << "Duration: " << std::chrono::duration_cast<pSeconds>(endTime - startTime).count() << "s\n";
    stats[0].print();
    delete[] stats;

    startTime = endTime;
  }

  auto trueEnd = std::chrono::steady_clock::now();
  std::cout << "Total duration: " <<
    std::chrono::duration_cast<pSeconds>(trueEnd - trueStart).count() << "s\n\n";

  // List n best surfaces.
  int n = 20;
  int tie = 1;
  float lastMax = 1.0e9;
  std::cout << "Top " << n << " surfaces\n\n";
  for(int i = 0; i < n; i++){
    float nextMax = 0.0;
    int nextStack = 0;
    int tCount = 0;
    for(int j = 0; j < NUM_SURFACES; j++){
      if(ranks[iters % 2][j] > nextMax){
        if(ranks[iters % 2][j] == lastMax){
          tCount++;
          if(tCount > tie){
            nextMax = ranks[iters % 2][j];
            nextStack = j;
            tie++;
            break;
          }
        }
        if(ranks[iters % 2][j] < lastMax){
          nextMax = ranks[iters % 2][j];
          nextStack = j;
        }
      }
    }
    if(nextMax < lastMax){
      tie = 1;
    }
    std::cout << printSurface(intToSurface(nextStack)) << "\n" << nextMax << "\n\n";
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

RankStats::RankStats():
  nSurfaces(0),
  bestSurface(0),
  bestRank(0.0f),
  maxErr(0.0f),
  totErr(0.0f),
  totSquErr(0.0f),
  totRank(0.0f){}

void RankStats::print(){
  double avgRank = totRank / (double) nSurfaces;
  double avgErr = totErr / (double) nSurfaces;;
  double avgSquErr = totSquErr / (double) nSurfaces;
  double stdErr = sqrt(avgSquErr - avgErr * avgErr);
  std::cout << "Avg rank: " << avgRank << "\n";
  std::cout << "Max error: " << maxErr << "\n";
  std::cout << "Avg error: " << avgErr << "\n";
  std::cout << "Standard deviation: " << stdErr << "\n\n";
  std::cout << "Best surface:\n";
  std::cout << printSurface(intToSurface(bestSurface)) << "\n" << bestRank << std::endl;
}

void RankStats::update(int row, int surface){
  int prev = (row + 1) % 2;
  nSurfaces++;
  if(ranks[row][surface] > bestRank){
    bestSurface = surface;
    bestRank = ranks[row][surface];
  }
  double err = (double) abs(ranks[row][surface] - ranks[prev][surface]);
  maxErr = std::max(maxErr, err);
  totErr += err;
  totSquErr += err * err;
  totRank += (double) ranks[row][surface];
}

void RankStats::merge(RankStats& otherStats){
  nSurfaces += otherStats.nSurfaces;
  if(bestRank < otherStats.bestRank){
    bestSurface = otherStats.bestSurface;
    bestRank = otherStats.bestRank;
  }
  maxErr = std::max(maxErr, otherStats.maxErr);
  totErr += otherStats.totErr;
  totSquErr += otherStats.totSquErr;
  totRank += otherStats.totRank;
}

void setRanks(int start, int end, int row, RankStats* stats){
  for(int i = start; i < end; i++){
    ranks[row][i] = rank2(row, i);
    stats->update(row, i);
  }
}

float rank2(int iter, int stack){
  int* surface = intToSurface(stack);
  float pieceRanks[] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

  for(int i = 0; i < SURFACE_WIDTH; i++){
    rankOrient2(iter, surface, I, 1, i, pieceRanks);
  }
  for(int i = 0; i < SURFACE_WIDTH - 1; i++){
    if(surface[i] == -2){
      rankOrient2(iter, surface, L, 3, i, pieceRanks);
    }
    if(surface[i] == -1){
      rankOrient2(iter, surface, T, 3, i, pieceRanks);
      rankOrient2(iter, surface, S, 1, i, pieceRanks);
    }
    if(surface[i] == 0){
      rankOrient2(iter, surface, O, 0, i, pieceRanks);
      rankOrient2(iter, surface, L, 1, i, pieceRanks);
      rankOrient2(iter, surface, J, 3, i, pieceRanks);
    }
    if(surface[i] == 1){
      rankOrient2(iter, surface, T, 1, i, pieceRanks);
      rankOrient2(iter, surface, Z, 1, i, pieceRanks);
    }
    if(surface[i] == 2){
      rankOrient2(iter, surface, J, 1, i, pieceRanks);
    }
  }
  for(int i = 0; i < SURFACE_WIDTH - 2; i++){
    if(surface[i] == 0 && surface[i + 1] == 0){
      rankOrient2(iter, surface, T, 2, i, pieceRanks);
      rankOrient2(iter, surface, J, 2, i, pieceRanks);
      rankOrient2(iter, surface, L, 2, i, pieceRanks);
    }
    if(surface[i] == -1 && surface[i + 1] == 1){
      rankOrient2(iter, surface, T, 0, i, pieceRanks);
    }
    if(surface[i] == 0 && surface[i + 1] == -1){
      rankOrient2(iter, surface, J, 0, i, pieceRanks);
    }
    if(surface[i] == -1 && surface[i + 1] == 0){
      rankOrient2(iter, surface, Z, 0, i, pieceRanks);
    }
    if(surface[i] == 0 && surface[i + 1] == 1){
      rankOrient2(iter, surface, S, 0, i, pieceRanks);
    }
    if(surface[i] == 1 && surface[i + 1] == 0){
      rankOrient2(iter, surface, L, 0, i, pieceRanks);
    }
  }

  for(int i = 0; i < SURFACE_WIDTH - 3; i++){
    if(surface[i] == 0 && surface[i + 1] == 0 && surface[i + 2] == 0){
      rankOrient2(iter, surface, I, 0, i, pieceRanks);
    }
  }

  delete[] surface;

  float pieceTotal = pieceRanks[0] + pieceRanks[1] + pieceRanks[2] + pieceRanks[3] + pieceRanks[4] + pieceRanks[5] + pieceRanks[6];
  return pieceTotal / 7.0f + 1.0f;
}

void rankOrient2(int iter, int* surface, int piece, int orientation, int pos, float*pieceRanks){
  iter = (iter + 1) % 2;
  int* newSurface = addPieceToSurface(surface, piece, orientation, pos, false);
  if(validSurface(newSurface)){
    int newNum = surfaceToInt(newSurface);
    float newRank = ranks[iter][newNum];
    pieceRanks[piece] = std::max(pieceRanks[piece], newRank);
  }
  delete[] newSurface;
}
