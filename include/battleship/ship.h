#ifndef SHIP_H
#define SHIP_H

#include "battleship/point.h"
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

class Ship {
public:
  // Number of cells occupied
  bool isVertical; // True if the ship is vertical
  int size;
  int hits;                 // Number of hits taken
  std::vector<Point> cells; // Cells occupied by the ship

  Ship(int s) : isVertical(false), size(s), hits(0){};
  Ship(){}; // 默认构造函数
  ~Ship(){};

  /**
   * @brief Check if the ship is sunk
   */
  bool isSunk() const;

  /**
   * @brief Add a cell to the ship
   */
  bool isHit(Point p);

  /**
   * @brief Add a cell to the ship
   */
  bool addCell(Point p);

  bool placed();

  /**
   * @brief Get the cells occupied by the ship
   */
  std::vector<Point> getCells() { return cells; }

  // 序列化
  string serialize();

  // 反序列化
  static Ship deserialize(const string &data);
};

ostream &operator<<(ostream &os, const Ship &ship);

#endif // SHIP_H