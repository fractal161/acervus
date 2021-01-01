#include "gameParts.hpp"
#include "gameState.hpp"
#include "search.hpp"
#include "controller.hpp"
#include <iostream>
#include <vector>
#include <chrono>

int main(){
  std::cout << "Lets play Tetris!\n\n";
  using pSeconds = std::chrono::duration<double, std::ratio<1, 1>>;
  auto trueStart = std::chrono::steady_clock::now();
  int n = 1; // 1000 for profiling
  int tScore = 0;
  srand(4304);
  Controller controller;
  for(int i = 0; i < n; i++){
    // std::random_device rd;
    // srand(rd());
    int seed = 0;
    // std::cout << "Using seed " << seed << "\n";
    BasicDfs pSearch;
    GameState state(seed, pSearch);
    bool gameOver = false;
    while(!gameOver){
      // std::cout << state.toString() << "\n";
      // std::cout << "Listing final resting positions..." << "\n";
      std::vector<Piece> places;
      state.search(places);
      // std::cout << state.toString() << "\n";
      for(auto p : places){
        std::cout << p.getX() << " " << p.getY() << " " << p.getOrient() << "\n";
      }
      Piece rest = controller.move(state);
      if(rest.getX() == -1){
        // std::cout << "Game over!\n";
        gameOver = true;
        break;
      }
      state.setPiece(rest);


      // std::cout << "Enter resting x-position: ";
      // int x, y, orient;
      // std::cin >> x;
      // std::cout << "Enter resting y-position: ";
      // std::cin >> y;
      // std::cout << "Enter resting orientation: ";
      // std::cin >> orient;
      // orient %= 4;
      // state.setCpos(x, y, orient);
      std::cout << state.toString() << "\n";
      std::cout << "Please confirm. ";
      std::cin.get();
      // std::cin.clear();
      // std::cin.get();
      state.place();
      state.update();
      // Do some stuff to lock and generate new pieces.
    }
    std::cout << state.score << "\n";
    tScore += state.score;
  }
  std::cout << "Average score: " << (double) tScore / n << "\n";
  auto trueEnd = std::chrono::steady_clock::now();
  std::cout << "Total duration: " <<
    std::chrono::duration_cast<pSeconds>(trueEnd - trueStart).count() << "s\n\n";
}
