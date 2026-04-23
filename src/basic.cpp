#include <iostream>

#include "server.h"

/**
 * This is the main function of the game. You don't need to modify it.
 * Just finish server.h and run!
 */
int main() {
  InitMap();
  PrintMap();
  while (true) {
    int pos_x, pos_y, type;
    // Read the coordinate and operation type. 0 for VisitBlock(x, y), 1 for MarkMine(x, y) and 2 for AutoExplore(x, y)
    if (!(std::cin >> pos_x >> pos_y >> type)) {
      break;  // End of input
    }
    
    if (type == 0) {
      VisitBlock(pos_x, pos_y);
    } else if (type == 1) {
      MarkMine(pos_x, pos_y);
    } else if (type == 2) {
      AutoExplore(pos_x, pos_y);
    }
    if (game_state == 1) {
      // Victory: show all mines as @ regardless of marked status
      for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
          if (is_mine[i][j]) {
            std::cout << '@';
          } else if (is_visited[i][j]) {
            std::cout << mine_count[i][j];
          } else {
            std::cout << '?';
          }
        }
        std::cout << std::endl;
      }
    } else {
      PrintMap();
    }
    
    if (game_state != 0) {
      ExitGame();
    }
  }
  return 0;
}
