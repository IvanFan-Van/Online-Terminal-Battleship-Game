#include "battleship/board.h"
#include "battleship/point.h"
#include "battleship/ship.h"
#include "common/game_action.h"
#include <iostream>
#include <string>

using namespace std;

int main() {
  // 测试 Point 类的序列化和反序列化
  Point point(1, 2);
  string serializedPoint = point.serialize();
  cout << "Serialized point: " << serializedPoint << endl;
  Point deserializedPoint = Point::deserialize(serializedPoint);
  cout << "Deserialized point: " << deserializedPoint.x << ","
       << deserializedPoint.y << endl;

  // 测试 Ship 类的序列化和反序列化
  cout << "===========SHIP=========\n";
  Ship ship(3);
  ship.addCell(Point(1, 2));
  ship.addCell(Point(3, 4));
  ship.addCell(Point(5, 6));
  string serializedShip = ship.serialize();
  cout << "Serialized ship: \n" << serializedShip << endl;
  Ship deserializedShip = Ship::deserialize(serializedShip);
  cout << "Deserialized ship: " << deserializedShip << endl;

  Ship ship2(2);
  ship2.addCell(Point(0, 0));
  ship2.addCell(Point(0, 1));
  string serializedShip2 = ship2.serialize();
  cout << "Serialized ship2: \n" << serializedShip2 << endl;
  Ship deserializedShip2 = Ship::deserialize(serializedShip2);
  cout << "Deserialized ship2: " << deserializedShip2 << endl;

  // 测试 Board 类的序列化和反序列化
  cout << "===========BOARD=========\n";
  Board board;
  Ship ship3(3);
  ship3.addCell(Point(1, 2));
  ship3.addCell(Point(1, 3));
  ship3.addCell(Point(1, 4));
  board.placeShip(ship3);
  Ship ship4(2);
  ship4.addCell(Point(3, 4));
  ship4.addCell(Point(4, 4));
  board.placeShip(ship4);
  // 如果反序列化的Board和原Board相同，说明序列化和反序列化正确
  string serializedBoard = board.serialize();
  cout << "Serialized board size: \n" << serializedBoard << endl;
  Board deserializedBoard = Board::deserialize(serializedBoard);
  cout << "Original board:\n" << board << endl;
  cout << "Deserialized board:\n" << deserializedBoard << endl;

  // 测试 GameAction 的序列化和反序列化
  cout << "====================\n";
  GameAction startAction(START);
  string serializedStartAction = startAction.serialize();
  cout << "Serialized startAction size: \n"
       << serializedStartAction.size() << endl;

  GameAction deserializedStartAction =
      GameAction::deserialize(serializedStartAction);
  // cout << "Deserialized Game Action  " << deserializedStartAction << endl;
  cout << "Deserialized Start Game Action Type: "
       << deserializedStartAction.type << endl;
  cout << "Deserialized Game Action data size: "
       << deserializedStartAction.data.size() << endl;

  // 测试 GameAction 的序列化和反序列化
  cout << "====================\n";
  Board board2;
  Ship ship5(3);
  ship5.addCell(Point(1, 2));
  ship5.addCell(Point(1, 3));
  ship5.addCell(Point(1, 4));
  board2.placeShip(ship5);
  Ship ship6(2);
  ship6.addCell(Point(3, 4));
  ship6.addCell(Point(4, 4));
  board2.placeShip(ship6);
  cout << "Original board2:" << endl;
  cout << board2;
  string serializedBoard2 = board2.serialize();
  GameAction initAction(INIT, serializedBoard2);
  string serializedInitAction = initAction.serialize();
  cout << "Serialized initAction size: " << serializedInitAction.size() << endl;

  GameAction deserializedInitAction =
      GameAction::deserialize(serializedInitAction);
  cout << "Deserialized Game Action Type: " << deserializedInitAction.type
       << endl;
  cout << "Deserialized Game Action data size: "
       << deserializedInitAction.data.size() << endl;

  Board deserializedBoard2 = Board::deserialize(deserializedInitAction.data);
  cout << "Deserialized board2 : \n" << deserializedBoard2;
  // deserializedBoard2.display(true);
}
