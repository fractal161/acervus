#include "gameParts.hpp"
#include <iostream>

Piece::Piece(){
  type = -1;
  x = -1;
  y = 0;
  orient = 0;
}

Piece::Piece(int nType){
  type = nType;
  x = 5;
  y = 0;
  orient = 0;
}

Piece::~Piece(){}

int Piece::numOrients() const{
  return orientMasks[type] + 1;
}

const std::array<std::pair<int, int>,4>& Piece::getOffs() const{
  return pCoords[type][orient];
}
int Piece::getX() const{
  return x;
}
int Piece::getY() const{
  return y;
}
int Piece::getType() const{
  return type;
}
int Piece::getOrient() const{
  return orient;
}
void Piece::shift(int off){
  x += off;
}
void Piece::rotate(int amt){
  orient = (orient + amt) & orientMasks[type];
}
void Piece::drop(int off){
  y += off;
}
void Piece::setPos(int nx, int ny, int norient){
  x = nx;
  y = ny;
  orient = norient;
}

void Piece::operator =(const Piece& p){
  type = p.type;
  x = p.x;
  y = p.y;
  orient = p.orient;
}

bool Piece::inBounds() const{
  // std::cout << x << " " << y <<  std::endl;
  // if(x < 0) return false;
  // const std::pair<int, int>& lims = pLimits2[type][orient];
  // return ((1 << y) & lims.first) != 0 && ((1 << x) & lims.second) != 0;

  const std::array<int, 4>& lims = pLimits[type][orient];
  return x >= lims[2] &&
    x <= lims[3] - 1 && // CHANGED
    // y >= lims[0] &&
    y <= lims[1];
}

//
uint64_t Piece::getBits() const{
  uint64_t bits = pBits[type][orient];
  // bits <<= 2;
  return bits >> x;
}

Board::Board(){
  cells = {0x3FF, 0x0, 0x0, 0x0};
  rows[0] = &cells[0];
  for(int i = 0; i < 20; i++){
    // Assign pointers to each row+stuff above row
    rows[i] = (uint64_t*)(((uint8_t*) rows[0]) + (5 * i) / 4);
  }
  maxCol = 0;
  maxColTmp = 0;
}

bool Board::getCell(int x, int y) const{
  int index = 209 - 10 * y - x;
  uint64_t mask = (uint64_t) 1 << (index & 63);
  return cells[(index >> 6)] & mask;
   // Dependent on coords
}

void Board::setCell(int x, int y, bool value){
  int index = 209 - 10 * y - x;
  if(value){
    uint64_t mask = (uint64_t) 1 << (index & 63);
    cells[(index >> 6)] |= mask;
  }
  else{
    uint64_t mask = ~((uint64_t) 1 << (index & 63));
    cells[(index >> 6)] &= mask;
  }
}

void printRows(uint64_t thing, int n){
  std::string result = "";
  for(int i = 0; i < n; i++){
    result = "\n" + result;
    for(int j = 0; j < 10; j++){
      int bit = thing % 2;
      char nxt = '0' + bit;
      result = nxt + result;
      thing >>= 1;
    }
  }
  std::cout << result;
}

bool Board::overlap(const Piece& piece) const{
  if(!piece.inBounds()) return true;

  // const uint64_t bits = ;
  int tmp = 19 - piece.getY();
  return (*rows[tmp] >> (((tmp) << 1) & 7)) & piece.getBits();
}


// Assumes placement is valid
void Board::place(const Piece& piece){
  const uint64_t bits = piece.getBits();
  int tmp = 19 - piece.getY();
  int offset = ((tmp) << 1) & 7;
  *rows[tmp] |= bits << offset;
  cells[3] &= 0x1FFFF;
  maxColTmp = maxCol;
  maxCol = std::max(maxCol, tmp + pHeights[piece.getType()][piece.getOrient()]);
}

void Board::remove(const Piece& piece){
  const uint64_t bits = piece.getBits();
  int tmp = 19 - piece.getY();
  int offset = ((tmp) << 1) & 7;
  *rows[tmp] &= ~(bits << offset);
  maxCol = maxColTmp;
}

bool Board::isEmpty() const{
  if(cells[0] >> 10){
    return false;
  }
  for(int i = 1; i < 4; i++){
    if(cells[i]){
      return false;
    }
  }
  return true;
}

int Board::clearRows(){
  int clear = 0;
  for(int i = HEIGHT - 1; i >= 0; i--){
    int tmp = 19 - i;

    if(((*rows[tmp] >> (10 + ((tmp << 1) & 7))) & 0x3FE) == 0x3FE){
      for(int j = i; j > 0; j--){
        for(int k = 0; k < WIDTH; k++){
          setCell(k, j, getCell(k, j - 1));
        }
      }
      for(int j = 0; j < WIDTH; j++){
        setCell(j, 0, false);
      }
      clear++;
      i++;
    }
  }
  maxCol -= clear;
  return clear;
}

void Board::toSurface(int* bumps){
  // int* bumps = new int[WIDTH - 2];
  // First column height.
  int col = 0;
  for(int i = HEIGHT - maxCol - 1; i < HEIGHT; i++){
    if(getCell(0, i)){
      col = HEIGHT - i;
      break;
    }
  }
  // Column deltas
  for(int j = 1; j < WIDTH - 1; j++){
    int nCol = 0;
    for(int i = HEIGHT - maxCol - 1; i < HEIGHT; i++){
      if(getCell(j, i)){
        nCol = HEIGHT - i;
        break;
      }
    }
    bumps[j - 1] = nCol - col;
    col = nCol;
  }
}

// #define DEBUG
#ifdef DEBUG
int main(){
  return 0;
}
#endif
