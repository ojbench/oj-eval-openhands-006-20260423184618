#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <utility>
#include <vector>
#include <set>

extern int rows;         // The count of rows of the game map.
extern int columns;      // The count of columns of the game map.
extern int total_mines;  // The count of mines of the game map.

// You MUST NOT use any other external variables except for rows, columns and total_mines.

// Client-side game state variables
std::vector<std::vector<char>> client_map;  // What the client sees
std::vector<std::vector<bool>> revealed;    // Which grids are revealed to client
std::set<std::pair<int, int>> safe_cells;   // Cells known to be safe
std::set<std::pair<int, int>> mine_cells;   // Cells known to be mines

/**
 * @brief The definition of function Execute(int, int, bool)
 *
 * @details This function is designed to take a step when player the client's (or player's) role, and the implementation
 * of it has been finished by TA. (I hope my comments in code would be easy to understand T_T) If you do not understand
 * the contents, please ask TA for help immediately!!!
 *
 * @param r The row coordinate (0-based) of the block to be visited.
 * @param c The column coordinate (0-based) of the block to be visited.
 * @param type The type of operation to a certain block.
 * If type == 0, we'll execute VisitBlock(row, column).
 * If type == 1, we'll execute MarkMine(row, column).
 * If type == 2, we'll execute AutoExplore(row, column).
 * You should not call this function with other type values.
 */
void Execute(int r, int c, int type);

/**
 * @brief The definition of function InitGame()
 *
 * @details This function is designed to initialize the game. It should be called at the beginning of the game, which
 * will read the scale of the game map and the first step taken by the server (see README).
 */
void InitGame() {
  // Initialize all client-side variables
  client_map.assign(rows, std::vector<char>(columns, '?'));
  revealed.assign(rows, std::vector<bool>(columns, false));
  safe_cells.clear();
  mine_cells.clear();
  
  int first_row, first_column;
  std::cin >> first_row >> first_column;
  Execute(first_row, first_column, 0);
}

/**
 * @brief The definition of function ReadMap()
 *
 * @details This function is designed to read the game map from stdin when playing the client's (or player's) role.
 * Since the client (or player) can only get the limited information of the game map, so if there is a 3 * 3 map as
 * above and only the block (2, 0) has been visited, the stdin would be
 *     ???
 *     12?
 *     01?
 */
void ReadMap() {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      char cell;
      std::cin >> cell;
      client_map[i][j] = cell;
      
      // Update revealed status
      if (cell != '?') {
        revealed[i][j] = true;
        
        // Update known safe and mine cells
        if (cell >= '0' && cell <= '8') {
          safe_cells.insert({i, j});
        } else if (cell == '@') {
          mine_cells.insert({i, j});
        }
      }
    }
  }
}

/**
 * @brief The definition of function Decide()
 *
 * @details This function is designed to decide the next step when playing the client's (or player's) role. Open up your
 * mind and make your decision here! Caution: you can only execute once in this function.
 */
void Decide() {
  // Strategy 1: Find obvious safe cells (auto-explore)
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (client_map[i][j] >= '1' && client_map[i][j] <= '8') {
        int mine_count = client_map[i][j] - '0';
        int unknown_count = 0;
        int marked_count = 0;
        std::vector<std::pair<int, int>> unknown_neighbors;
        
        // Count neighbors
        int dr[] = {-1, -1, -1, 0, 0, 1, 1, 1};
        int dc[] = {-1, 0, 1, -1, 1, -1, 0, 1};
        
        for (int k = 0; k < 8; k++) {
          int ni = i + dr[k];
          int nj = j + dc[k];
          if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
            if (client_map[ni][nj] == '?') {
              unknown_count++;
              unknown_neighbors.push_back({ni, nj});
            } else if (client_map[ni][nj] == '@') {
              marked_count++;
            }
          }
        }
        
        // If all remaining neighbors are mines, mark them
        if (unknown_count > 0 && marked_count + unknown_count == mine_count) {
          for (auto& cell : unknown_neighbors) {
            if (mine_cells.find(cell) == mine_cells.end()) {
              Execute(cell.first, cell.second, 1);  // Mark mine
              return;
            }
          }
        }
        
        // If all mines are already marked, visit remaining unknown neighbors
        if (marked_count == mine_count && unknown_count > 0) {
          Execute(i, j, 2);  // Auto-explore
          return;
        }
      }
    }
  }
  
  // Strategy 2: Find cells that must be safe using logical deduction
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (client_map[i][j] >= '1' && client_map[i][j] <= '8') {
        int mine_count = client_map[i][j] - '0';
        int unknown_count = 0;
        int marked_count = 0;
        std::vector<std::pair<int, int>> unknown_neighbors;
        
        // Count neighbors
        int dr[] = {-1, -1, -1, 0, 0, 1, 1, 1};
        int dc[] = {-1, 0, 1, -1, 1, -1, 0, 1};
        
        for (int k = 0; k < 8; k++) {
          int ni = i + dr[k];
          int nj = j + dc[k];
          if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
            if (client_map[ni][nj] == '?') {
              unknown_count++;
              unknown_neighbors.push_back({ni, nj});
            } else if (client_map[ni][nj] == '@') {
              marked_count++;
            }
          }
        }
        
        // If all unknown neighbors must be safe, visit one
        if (marked_count == mine_count && unknown_count > 0) {
          for (auto& cell : unknown_neighbors) {
            if (safe_cells.find(cell) == safe_cells.end()) {
              Execute(cell.first, cell.second, 0);  // Visit safe cell
              return;
            }
          }
        }
      }
    }
  }
  
  // Strategy 3: Pattern-based deduction for more complex situations
  // Check for 1-2 patterns and other common Minesweeper patterns
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (client_map[i][j] >= '1' && client_map[i][j] <= '8') {
        int mine_count = client_map[i][j] - '0';
        std::vector<std::pair<int, int>> unknown_neighbors;
        std::vector<std::pair<int, int>> marked_neighbors;
        
        // Get all neighbors
        int dr[] = {-1, -1, -1, 0, 0, 1, 1, 1};
        int dc[] = {-1, 0, 1, -1, 1, -1, 0, 1};
        
        for (int k = 0; k < 8; k++) {
          int ni = i + dr[k];
          int nj = j + dc[k];
          if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
            if (client_map[ni][nj] == '?') {
              unknown_neighbors.push_back({ni, nj});
            } else if (client_map[ni][nj] == '@') {
              marked_neighbors.push_back({ni, nj});
            }
          }
        }
        
        // Try to find safe cells through constraint satisfaction
        if (!unknown_neighbors.empty() && (int)marked_neighbors.size() < mine_count) {
          // Simple heuristic: choose the unknown neighbor with the lowest probability of being a mine
          int best_score = 1000;
          std::pair<int, int> best_cell = {-1, -1};
          
          for (auto& cell : unknown_neighbors) {
            int score = 0;
            int total_neighbors = 0;
            int mine_neighbors = 0;
            
            // Count neighbors of this unknown cell
            for (int k = 0; k < 8; k++) {
              int ni = cell.first + dr[k];
              int nj = cell.second + dc[k];
              if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
                total_neighbors++;
                if (client_map[ni][nj] >= '1' && client_map[ni][nj] <= '8') {
                  mine_neighbors += client_map[ni][nj] - '0';
                }
              }
            }
            
            if (total_neighbors > 0) {
              score = mine_neighbors * 1000 / total_neighbors;
            }
            
            if (score < best_score) {
              best_score = score;
              best_cell = cell;
            }
          }
          
          if (best_cell.first != -1) {
            Execute(best_cell.first, best_cell.second, 0);  // Visit safest looking cell
            return;
          }
        }
      }
    }
  }
  
  // Strategy 4: Random selection as last resort
  std::vector<std::pair<int, int>> unknown_cells;
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (client_map[i][j] == '?') {
        unknown_cells.push_back({i, j});
      }
    }
  }
  
  if (!unknown_cells.empty()) {
    // Choose a random unknown cell
    int index = (unknown_cells.size() * 7 + 13) % unknown_cells.size();  // Simple pseudo-random
    Execute(unknown_cells[index].first, unknown_cells[index].second, 0);
  }
}

#endif