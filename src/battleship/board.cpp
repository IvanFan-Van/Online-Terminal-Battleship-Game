#include "battleship/board.h"
#include "common/color.h"
#include "common/constants.h"
#include "common/utilities.h"
#include <iostream>
#include <sstream>

using namespace std;

Board::Board() {
  for (int i = 0; i < BOARD_SIZE; ++i) {
    for (int j = 0; j < BOARD_SIZE; ++j) {
      board[i][j] = EMPTY;
      hits[i][j] = false;
    }
  }
}

Board::Board(char board[BOARD_SIZE][BOARD_SIZE]) {
  for (int i = 0; i < BOARD_SIZE; ++i) {
    for (int j = 0; j < BOARD_SIZE; ++j) {
      this->board[i][j] = board[i][j];
      hits[i][j] = false;
    }
  }
}

void Board::display(bool showShips) {
  cout << "  0 1 2 3 4 5 6 7 8 9\n";
  for (int i = 0; i < BOARD_SIZE; ++i) {
    cout << char('A' + i) << ' ';
    for (int j = 0; j < BOARD_SIZE; ++j) {
      // Show hits or misses accordingly
      if (hits[i][j]) {
        if (board[i][j] == HIT) {
          cout << HIT_CELL << ' ';
        } else {
          cout << MISS_CELL << ' ';
        }
      } else if (showShips && board[i][j] == SHIP) {
        cout << SHIP_CELL << ' '; // Show ships if they are supposed to be shown
      } else {
        cout << EMPTY_CELL << ' '; // Show empty space otherwise
      }
    }
    cout << endl;
  }
}

bool Board::isValidPlacement(int x, int y, int size, bool isVertical) {
  // 加入检查相邻格子的逻辑
  if (isVertical) {
    if (x + size > BOARD_SIZE || y + 1 > BOARD_SIZE)
      return false;
    for (int i = x - 1; i <= x + size; ++i) {
      for (int j = y - 1; j <= y + 1; ++j) {
        if (i >= 0 && i < BOARD_SIZE && j >= 0 && j < BOARD_SIZE &&
            board[i][j] != EMPTY)
          return false;
      }
    }
  } else {
    if (y + size > BOARD_SIZE || x + 1 > BOARD_SIZE)
      return false;
    for (int i = x - 1; i <= x + 1; ++i) {
      for (int j = y - 1; j <= y + size; ++j) {
        if (i >= 0 && i < BOARD_SIZE && j >= 0 && j < BOARD_SIZE &&
            board[i][j] != EMPTY)
          return false;
      }
    }
  }
  return true;
}

bool Board::allShipsPlaced() {
  for (int i = 0; i < BOARD_SIZE; ++i) {
    for (int j = 0; j < BOARD_SIZE; ++j) {
      if (board[i][j] == SHIP) {
        return true;
      }
    }
  }
  return false;
}

void Board::placeShip(Ship &ship) {
  for (int i = 0; i < ship.size; i++) {
    Point p = ship.cells[i];
    board[p.x][p.y] = SHIP;
  }
  ships.push_back(ship);
}

void Board::placeShip(int x, int y, int size, bool isVertical) {
  Ship ship(size);
  ship.isVertical = isVertical;
  if (isVertical) {
    for (int i = x; i < x + size; ++i) {
      board[i][y] = SHIP;
      ship.addCell({i, y});
    }
  } else {
    for (int i = y; i < y + size; ++i) {
      board[x][i] = SHIP;
      ship.addCell({x, i});
    }
  }
  ships.push_back(ship);
}

bool Board::handleHit(int x, int y) {
  if (hits[x][y]) {
    // Position was already hit before
    return false;
  }
  // Record the hit attempt
  hits[x][y] = true;
  if (board[x][y] == SHIP) {
    // 更新板上的位置为HIT标记
    board[x][y] = HIT;

    // Mark diagonal positions as MISS
    int diagonalOffsets[] = {-1, 1};
    for (int dx : diagonalOffsets) {
      for (int dy : diagonalOffsets) {
        int newX = x + dx;
        int newY = y + dy;

        if (newX >= 0 && newX < BOARD_SIZE && newY >= 0 && newY < BOARD_SIZE &&
            board[newX][newY] == EMPTY) {
          board[newX][newY] = MISS;
          hits[newX][newY] = true;
        }
      }
    }

    // Check if any ship is sunk
    for (Ship &ship : ships) {
      if (ship.isHit({x, y}) && ship.isSunk()) {
        // Mark the surroundings of the sunk ship as MISS
        for (Point &cell : ship.cells) {
          for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
              int newX = cell.x + dx;
              int newY = cell.y + dy;
              if (newX >= 0 && newX < BOARD_SIZE && newY >= 0 &&
                  newY < BOARD_SIZE && !hits[newX][newY]) {
                board[newX][newY] = MISS;
                hits[newX][newY] = true;
              }
            }
          }
        }
        return true;
      }
    }

    return true;
  } else {
    board[x][y] = MISS; // Update the board with a MISS marker
    return false;
  }
}

bool Board::allShipsSunk() {
  for (int i = 0; i < BOARD_SIZE; ++i) {
    for (int j = 0; j < BOARD_SIZE; ++j) {
      if (board[i][j] == SHIP) {
        return false;
      }
    }
  }
  return true;
}

bool Board::isOccupied(int x, int y) { return board[x][y] != EMPTY; }

bool Board::isHit(int x, int y) { return hits[x][y]; }

void Board::displayRow(int row, bool showShips, int col) const {
  for (int j = 0; j < BOARD_SIZE; ++j) {
    if (!showShips && col == j) {
      if (!hits[row][j]) {
        cout << GREEN << '#' << ' ' << RESET_COLOR;
      } else {
        cout << RED_BG << '#' << RESET_COLOR << ' ';
      }
    } else if (showShips || hits[row][j]) {
      if (hits[row][j]) {
        if (board[row][j] == HIT) {
          cout << HIT_CELL << ' ';
        } else {
          cout << MISS_CELL << ' ';
        }
      } else if (board[row][j] == SHIP) {
        cout << SHIP_CELL << ' ';
      } else {
        cout << EMPTY_CELL << ' ';
      }
    } else {
      cout << EMPTY_CELL << ' ';
    }
  }
}

Point Board::getRandomPoint() {
  Point p;
  p.x = rand() % BOARD_SIZE;
  p.y = rand() % BOARD_SIZE;
  return p;
}

void Board::DisplayColorPlacement(int x, int y, int size, bool isVertical) {
  clearScreen();
  cout << "Place a ship of size " << CYAN << BOLD << size << RESET_COLOR << endl
       << endl;
  cout << "Please use " << ITALIC << UNDERLINE << "wasd " << RESET_COLOR
       << "or " << ITALIC << UNDERLINE << "arrow keys" << RESET_COLOR
       << " to move your ship, \nOr press the " << BOLD << UNDERLINE << BLINKING
       << "[  spacebar  ]" << RESET_COLOR
       << " to change its orientation. \nNote: upperleft corner will be "
          "fixed during rotation."
       << endl
       << endl;
  cout << "When the ship is " << RED_BG << "red" << RESET_COLOR
       << ", it is an invalid placement." << endl;
  cout << "When it is " << BOLD << GREEN << "green" << RESET_COLOR
       << ", it is valid!" << endl;
  cout << "Press the " << BOLD << CYAN << "Enter" << RESET_COLOR
       << " key to place this ship." << endl
       << endl;

  cout << "  0 1 2 3 4 5 6 7 8 9\n";
  bool isValid = isValidPlacement(x, y, size, isVertical);
  for (int i = 0; i < BOARD_SIZE; ++i) {
    cout << char('A' + i) << ' ';
    for (int j = 0; j < BOARD_SIZE; ++j) {
      if (isVertical) { // when the ship is vertical:
        if (x <= i && i < x + size &&
            y == j) { // check if the ij-th block is under the ship to be placed
          if (j < BOARD_SIZE - 1) { // if the block is not at the rightmost end,
                                    // add a space after it
            if (!(isValid)) {
              cout << RED_BG << SHIP << RESET_COLOR << ' ';
            } else {
              cout << GREEN << SHIP << ' ' << RESET_COLOR;
            }
          } else {
            if (!(isValid)) { // at the rightmost end -> no space added
              cout << RED_BG << SHIP << RESET_COLOR;
            } else {
              cout << GREEN << SHIP << RESET_COLOR;
            }
          }
        } else {
          if (j < BOARD_SIZE - 1)
            cout << board[i][j] << ' ';
          else {
            cout << board[i][j];
          }
        }
      } else {
        if (y <= j && j < y + size && x == i) {
          if (j < BOARD_SIZE - 1) {
            if (!(isValid)) {
              if (j < y + size - 1) {
                cout << RED_BG << SHIP << ' ' << RESET_COLOR;
              } else {
                cout << RED_BG << SHIP << RESET_COLOR << ' ';
              }

            } else {
              cout << GREEN << SHIP << ' ' << RESET_COLOR;
            }
          } else {
            if (!(isValid)) {
              cout << RED_BG << SHIP << RESET_COLOR;
            } else {
              cout << GREEN << SHIP << RESET_COLOR;
            }
          }
        } else {
          if (j < BOARD_SIZE - 1)
            cout << board[i][j] << ' ';
          else {
            cout << board[i][j];
          }
        }
      }
    }
    cout << endl;
  }
}

Board::Board(const Board &other) { copyFrom(other); }

void Board::copyFrom(const Board &other) {
  for (int i = 0; i < BOARD_SIZE; ++i) {
    for (int j = 0; j < BOARD_SIZE; ++j) {
      board[i][j] = other.board[i][j];
      hits[i][j] = other.hits[i][j];
    }
  }
  ships = other.ships;
}

string Board::serialize() {
  ostringstream oss;
  for (int i = 0; i < BOARD_SIZE; ++i) {
    for (int j = 0; j < BOARD_SIZE; ++j) {
      oss << board[i][j] << " ";
      oss << hits[i][j] << " ";
    }
  }
  oss << "\n";

  for (Ship &ship : ships) {
    oss << ship.serialize() << "|";
  }
  return oss.str();
}

Board Board::deserialize(const string &data) {
  istringstream iss(data);
  Board board;
  for (int i = 0; i < BOARD_SIZE; ++i) {
    for (int j = 0; j < BOARD_SIZE; ++j) {
      iss >> board.board[i][j];
      iss.get(); // skip '\n'
      iss >> board.hits[i][j];
      iss.get(); // skip '\n'
    }
  }
  iss.get(); // skip '\n'

  string shipData;
  while (getline(iss, shipData, '|')) {
    board.ships.push_back(Ship::deserialize(shipData));
  }
  return board;
}

ostream &operator<<(ostream &os, const Board &b) {
  for (int i = 0; i < BOARD_SIZE; ++i) {
    for (int j = 0; j < BOARD_SIZE; ++j) {
      os << b.board[i][j] << " ";
    }
    os << endl;
  }
  cout << "Ships: " << endl;
  for (const Ship &ship : b.ships) {
    os << ship << endl;
  }
  return os;
}