#include "battleship/ship.h"

bool Ship::isHit(Point p) {
  for (Point &cell : cells) {
    if (cell == p) {
      hits++;
      return true;
    }
  }
  return false;
}

bool Ship::addCell(Point p) {
  if ((int)cells.size() < size) {
    cells.push_back(p);
    return true;
  }
  return false;
}

bool Ship::placed() { return (int)cells.size() == size; };

/**
 * @brief Check if the ship is sunk
 */
bool Ship::isSunk() const { return hits == size; }

// 序列化
string Ship::serialize() {
  ostringstream oss;
  oss << size << " " << hits << " " << isVertical << "\n";
  for (const auto &cell : cells) {
    oss << cell.serialize() << "\n";
  }
  return oss.str();
}

// 反序列化
Ship Ship::deserialize(const string &data) {
  istringstream iss(data);
  int size;
  iss >> size;
  Ship ship(size);
  iss >> ship.hits >> ship.isVertical;
  iss.get(); // skip '\n'
  string cellData;
  while (getline(iss, cellData)) {
    ship.addCell(Point::deserialize(cellData));
  }
  return ship;
}

ostream &operator<<(ostream &os, const Ship &ship) {
  os << "Size: " << ship.size << ", Hits: " << ship.hits
     << ", Vertical: " << ship.isVertical << ", Cells: ";
  for (const auto &cell : ship.cells) {
    os << cell << ";";
  }
  return os;
}