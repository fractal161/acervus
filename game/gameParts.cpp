#include "gameParts.hpp"

Piece::Piece(){
  type = -1;
  // pos = {{-1, 0, 0}};
  x = -1;
  y = 0;
  orient = 0;
}

Piece::Piece(int nType){
  type = nType;
  // pos = {{5, 0, 0}};
  x = 5;
  y = 0;
  orient = 0;
}

Piece::~Piece(){}

int Piece::numOrients() const{
  return nOrients[type];
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
int Piece::getOrient() const{
  return orient;
}
void Piece::shift(int off){
  x += off;
}
void Piece::rotate(int amt){
  orient = (orient + amt) % nOrients[type];
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

Board::Board(){
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      cells[i][j] = false;
    }
  }
  // cells[HEIGHT - 1][0] = true;
  // cells[HEIGHT - 1][WIDTH - 2] = true;
}

bool Board::getCell(int y, int x) const{
  return cells[x][y]; // Dependent on coords
}
void Board::setCell(int y, int x){
  cells[x][y] = true; // Dependent on coords
}

bool Board::overlap(const Piece& piece) const{
  const std::array<std::pair<int, int>,4> pOffs = piece.getOffs();
  int px = piece.getX();
  int py = piece.getY();
  for(auto& off : pOffs){
    int x = off.second + px;
    int y = off.first + py;
    if(x < 0 || x > WIDTH - 2 || //CHANGED FROM NORMAL
      y < -2 || y > HEIGHT - 1 ||
      (y >= 0 && cells[y][x])) return true;
      // ^ unnecessary without the change
  }
  return false;
}


// Assumes placement is valid
void Board::place(const Piece& piece){
  std::array<std::pair<int, int>,4> pOffs = piece.getOffs();
  int px = piece.getX();
  int py = piece.getY();
  for(auto& off : pOffs){
    cells[off.first + py][off.second + px] = true;
  }
}

void Board::remove(const Piece& piece){
  std::array<std::pair<int, int>,4> pOffs = piece.getOffs();
  int px = piece.getX();
  int py = piece.getY();
  for(auto& off : pOffs){
    cells[off.first + py][off.second + px] = false;
  }
}

int Board::clearRows(){
  int clear = 0;
  for(int i = HEIGHT - 1; i >= 0; i--){
    bool full = true;
    for(int j = 0; j < WIDTH - 1; j++){ // CHANGED
      if(!cells[i][j]){
        full = false;
        break;
      }
    }
    if(full){
      for(int j = i; j > 0; j--){
        for(int k = 0; k < WIDTH; k++){
          cells[j][k] = cells[j - 1][k];
        }
      }
      for(int j = 0; j < WIDTH; j++){
        cells[0][j] = false;
      }
      clear++;
      i++;
    }
  }
  return clear;
}

void Board::toSurface(int* bumps){
  // int* bumps = new int[WIDTH - 2];
  // First column height.
  int col = 0;
  for(int i = 0; i < HEIGHT; i++){
    if(cells[i][0]){
      col = HEIGHT - i;
      break;
    }
  }
  // Column deltas
  for(int j = 1; j < WIDTH - 1; j++){
    int nCol = 0;
    for(int i = 0; i < HEIGHT; i++){
      if(cells[i][j]){
        nCol = HEIGHT - i;
        break;
      }
    }
    bumps[j - 1] = nCol - col;
    col = nCol;
  }
  // return bumps;
}

// #define DEBUG
#ifdef DEBUG
int main(){
  return 0;
}
#endif
