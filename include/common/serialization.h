// /**
//  * @file serialization.h
//  * 为项目中的自定义类提供序列化和反序列化函数
//  */
// #ifndef SERIALIZATION_H
// #define SERIALIZATION_H

// #include <algorithm>
// #include <iostream>
// #include <sstream>

// using namespace std;

// std::string removeExtraNulls(const std::string &input) {
//   std::string output = input;
//   // 移除除了字符串末尾之外的所有 '\0' 字符
//   output.erase(std::remove(output.begin(), output.end() - 1, '\0'),
//                output.end() - 1);
//   return output;
// }

// // Point 类的序列化和反序列化
// #include "battleship/point.h"
// string serializePoint(const Point &point) {
//   return to_string(point.x) + "," + to_string(point.y);
// }

// Point deserializePoint(const string &data) {
//   size_t commaPos = data.find(',');
//   int x = stoi(data.substr(0, commaPos));
//   int y = stoi(data.substr(commaPos + 1));
//   return Point(x, y);
// }

// // Ship 类的序列化和反序列化
// #include "battleship/ship.h"
// string serializeShip(const Ship &ship) {
//   string serialized;
//   serialized += to_string(ship.size) + ",";
//   serialized += to_string(ship.hits) + ",";
//   serialized += to_string(ship.isVertical);
//   serialized += "," + to_string(ship.cells.size()) + ";";
//   for (const auto &cell : ship.cells) {
//     serialized += serializePoint(cell) + ";";
//   }
//   return serialized;
// }

// Ship deserializeShip(const string &data) {
//   size_t commaPos = data.find(',');
//   size_t size = stoi(data.substr(0, commaPos));
//   Ship ship(size);

//   size_t prevCommaPos = commaPos;
//   commaPos = data.find(',', prevCommaPos + 1);
//   ship.hits = stoi(data.substr(prevCommaPos + 1, commaPos));

//   prevCommaPos = commaPos;
//   commaPos = data.find(',', prevCommaPos + 1);
//   ship.isVertical = data.substr(prevCommaPos + 1, commaPos) == "1";

//   prevCommaPos = commaPos;
//   commaPos = data.find(';', prevCommaPos + 1);
//   int cellSize = stoi(data.substr(prevCommaPos + 1, commaPos));

//   size_t prevSemicolonPos = commaPos;
//   for (int i = 0; i < cellSize; i++) {
//     commaPos = data.find(';', prevSemicolonPos + 1);
//     ship.cells.push_back(
//         deserializePoint(data.substr(prevSemicolonPos + 1, commaPos)));
//     prevSemicolonPos = commaPos;
//   }

//   return ship;
// }

// // Board 类的序列化和反序列化
// #include "battleship/board.h"
// string serializeBoard(const Board &board) {
//   string serialized;
//   for (int i = 0; i < BOARD_SIZE; i++) {
//     for (int j = 0; j < BOARD_SIZE; j++) {
//       serialized += board.board[i][j];
//       serialized += board.hits[i][j];
//       serialized += ",";
//     }
//   }
//   serialized += to_string(board.ships.size()) + ";";
//   for (const auto &ship : board.ships) {
//     serialized += serializeShip(ship) + ";";
//   }
//   return removeExtraNulls(serialized);
// }

// Board deserializeBoard(const string &data) {
//   Board board;
//   size_t pos = 0;
//   for (int i = 0; i < BOARD_SIZE; i++) {
//     for (int j = 0; j < BOARD_SIZE; j++) {
//       size_t nextPos = data.find(',', pos);
//       board.board[i][j] = data.substr(pos, nextPos - pos)[0];
//       board.hits[i][j] = data.substr(pos, nextPos - pos)[1] == '1';
//       pos = nextPos + 1;
//     }
//   }

//   size_t nextPos = data.find(';', pos);
//   int shipSize = stoi(data.substr(pos, nextPos - pos));
//   pos = nextPos + 1;
//   for (int i = 0; i < shipSize; i++) {
//     nextPos = data.find(';', pos);
//     board.ships.push_back(deserializeShip(data.substr(pos, nextPos - pos)));
//     pos = nextPos + 1;
//   }

//   return board;
// }

// #endif // SERIALIZATION_H