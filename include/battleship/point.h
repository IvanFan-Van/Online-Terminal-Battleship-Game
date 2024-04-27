#ifndef POINT_H
#define POINT_H
#include <iostream>
#include <sstream>

using namespace std;

class Point {
public:
  int x;
  int y;

  // 构造函数
  Point(int x, int y) : x(x), y(y) {}
  // 默认构造函数
  Point(){};

  // 拷贝赋值操作符
  Point &operator=(const Point &other);

  // 比较操作符
  bool operator==(const Point &p) const;

  // 序列化
  string serialize() const;

  // 反序列化
  static Point deserialize(const string &data);
};

ostream &operator<<(ostream &os, const Point &p);

#endif // POINT_H