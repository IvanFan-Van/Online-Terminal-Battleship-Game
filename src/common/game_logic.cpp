#include "common/game_logic.h"
#include "battleship/keyboard.h"
#include "common/utilities.h"
#include <iostream>
#include <regex>

using namespace std;

bool GameLogic::placeShips(Board &board, Ship &ship) {
  int shipSize = ship.size;
  char key;
  bool placed = false;
  static int prev_x = -1, prev_y = -1;
  int x = prev_x == -1 ? STARTPOINT : prev_x;
  int y = prev_y == -1 ? STARTPOINT : prev_y;
  bool isVertical = true;
  board.displayColorPlacement(x, y, shipSize, isVertical);
  while (!placed) {
    key = keyboard();
    switch (key) {
    case 'w':
      if (x > 0) {
        x -= 1;
        board.displayColorPlacement(x, y, shipSize, isVertical);
      }
      break;
    case 'a':
      if (y > 0) {
        y -= 1;
        board.displayColorPlacement(x, y, shipSize, isVertical);
      }
      break;
    case 's':
      if (x < BOARD_SIZE - (isVertical ? shipSize : 1)) {
        x += 1;
        board.displayColorPlacement(x, y, shipSize, isVertical);
      }
      break;
    case 'd':
      if (y < BOARD_SIZE - (isVertical ? 1 : shipSize)) {
        y += 1;
        board.displayColorPlacement(x, y, shipSize, isVertical);
      }
      break;
    case 'C': // change orientation
      if ((isVertical ? y : x) + shipSize <= BOARD_SIZE) {
        isVertical = !isVertical;
        board.displayColorPlacement(x, y, shipSize, isVertical);
      }
      break;
    case 'Y': // choice made
      if (board.isValidPlacement(x, y, shipSize, isVertical)) {
        placed = true;
        if (isVertical) {
          for (int i = x; i < x + shipSize; ++i) {
            ship.addCell({i, y});
          }
        } else {
          for (int i = y; i < y + shipSize; ++i) {
            ship.addCell({x, i});
          }
        }
        // 将船放入船队
        board.placeShip(ship);

        // 更新之前的x, y坐标
        prev_x = x;
        prev_y = y;
      }
      break;
    case 'N': // 加了个中途退出功能，此处待定
      placed = true;
      return false;
      break;
    }
  }
  return true;
}

bool GameLogic::getMoveFromPlayer(Board playerBoard, Board opponentBoard,
                                  int &x, int &y) {
  bool placed = false;
  static int prev_x = -1, prev_y = -1;
  x = prev_x == -1 ? STARTPOINT : prev_x,
  y = prev_y == -1 ? STARTPOINT : prev_y;
  displayBoardsSideBySide(playerBoard, opponentBoard, true, x, y, false);
  while (!placed) {
    char key = keyboard();
    switch (key) {
    case 'w':
      if (x > 0) {
        x -= 1;
        displayBoardsSideBySide(playerBoard, opponentBoard, true, x, y, false);
      }
      break;
    case 'a':
      if (y > 0) {
        y -= 1;
        displayBoardsSideBySide(playerBoard, opponentBoard, true, x, y, false);
      }
      break;
    case 's':
      if (x < BOARD_SIZE - 1) {
        x += 1;
        displayBoardsSideBySide(playerBoard, opponentBoard, true, x, y, false);
      }
      break;
    case 'd':
      if (y < BOARD_SIZE - 1) {
        y += 1;
        displayBoardsSideBySide(playerBoard, opponentBoard, true, x, y, false);
      }
      break;
    case 'Y': // choice made
      if (!opponentBoard.isHit(x, y)) {
        placed = true;
      }
      // update previous move
      prev_x = x;
      prev_y = y;
      break;
    case 'N': // 加了个中途退出功能，此处待定
      placed = true;
      return false;
      break;
    }
  }
  return true;
}