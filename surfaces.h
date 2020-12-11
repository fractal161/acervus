#pragma once
#include <math.h>
#include <time.h>
#include <vector>
#include <cstring>
// #include <unistd.h>
#include <string>

extern const int SURFACE_WIDTH;
extern const std::vector<std::vector<std::vector<int>>> orients;


int * intToSurface(int surface);
int surfaceToInt(int * surface);
std::string printSurface(int * surface);
bool validSurface(int * surface);
int * createRandSurface();
int * addPieceToSurface(int * surface, int piece, int orientation, int position);
