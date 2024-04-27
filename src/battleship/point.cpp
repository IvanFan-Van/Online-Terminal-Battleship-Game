#include "battleship/point.h"

// 拷贝赋值操作符
Point &Point::operator=(const Point &other) {
  if (this != &other) { // 防止自赋值
    x = other.x;
    y = other.y;
  }
  return *this;
}

// 比较操作符
bool Point::operator==(const Point &p) const { return x == p.x && y == p.y; }

// 序列化
string Point::serialize() const {
  ostringstream oss;
  oss << x << " " << y;
  return oss.str();
}

// 反序列化
Point Point::deserialize(const string &data) {
  istringstream iss(data);
  Point p;
  iss >> p.x >> p.y;
  return p;
}

ostream &operator<<(ostream &os, const Point &p) {
  os << "(" << p.x << ", " << p.y << ")";
  return os;
}