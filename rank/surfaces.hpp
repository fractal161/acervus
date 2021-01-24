#pragma once
#include <cmath>
#include <time.h>
#include <vector>
#include <cstring>
#include <string>

const int SURFACE_WIDTH = 9;
enum Pieces {T, J, Z, O, S, L, I};
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



int* intToSurface(int surface);
int surfaceToInt(int* surface);
std::string printSurface(int* surface);
std::string printSurfaceAndPiece(int* surface, int type);
bool validSurface(int* surface);
int* createRandSurface();
int* addPieceToSurface(int* surface, int piece, int orientation, int position, bool check);
