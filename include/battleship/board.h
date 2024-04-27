#ifndef BOARD_H
#define BOARD_H

#include "battleship/ship.h"
#include "common/constants.h"
#include "point.h"
#include <vector>

class Board {
private:
  void printInstructions(int size) const;

  void printBoardHeader() const;

  void printBoardWithPlacement(int x, int y, int size, bool isVertical,
                               bool isValid) const;

  void printShipPlacement(int i, int j, int x, int y, int size, bool isVertical,
                          bool isValid) const;

public:
  char board[BOARD_SIZE][BOARD_SIZE];
  bool hits[BOARD_SIZE][BOARD_SIZE];
  std::vector<Ship> ships;

  Board();
  // 拷贝构造函数
  Board(const Board &other);

  // 拷贝赋值操作符
  Board &operator=(const Board &other) {
    if (this != &other) { // 防止自赋值
      copyFrom(other);
    }
    return *this;
  }

  Board(char board[BOARD_SIZE][BOARD_SIZE]);
  void display(bool showShips);

  bool isValidPlacement(int x, int y, int size, bool isVertical);

  void placeShip(Ship &ship);

  void placeShip(int x, int y, int size, bool isVertical);

  bool handleHit(int x, int y);

  bool allShipsSunk();

  bool isOccupied(int x, int y);

  bool isHit(int x, int y);

  void displayRow(int row, bool showShips, int col) const;

  bool allShipsPlaced();

  Point getRandomPoint();

  void displayColorPlacement(int x, int y, int size, bool isVertical);

  // 序列化
  string serialize();
  // 反序列化
  static Board deserialize(const string &data);

private:
  void copyFrom(const Board &other);
};

ostream &operator<<(ostream &os, const Board &board);

#endif // BOARD_H