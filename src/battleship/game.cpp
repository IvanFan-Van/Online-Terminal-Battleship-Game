#include "battleship/game.h"
#include "battleship/keyboard.h"
#include "common/action.h"
#include "common/color.h"
#include "common/constants.h"
#include "common/utilities.h"
#include <fstream>
#include <ios>
#include <iostream>
#include <limits>
#include <regex>
#include <sstream>
#include <unistd.h>

using namespace std;

void Game::placeShips(Board &board, bool isPlayer) {
  for (int shipSize : SHIPS) {
    // 创建一个船对象
    Ship ship(shipSize);
    if (isPlayer) {
      gameLogic.placeShips(board, ship);
    } else {
      while (true) {
        Point p = board.getRandomPoint();
        bool isVertical = rand() % 2 == 0;
        if (board.isValidPlacement(p.x, p.y, shipSize, isVertical)) {
          board.placeShip(p.x, p.y, shipSize, isVertical);
          break;
        }
      }
    }
  }
}

bool Game::loadGame(string filepath) {
  cout << "Loading game...\n";
  ifstream fin;
  fin.open(filepath);
  stringstream buffer;
  buffer << fin.rdbuf();
  string content = buffer.str();

  if (content.empty()) {
    cout << RED << "File is empty! or Not exits!\n" << RESET_COLOR;
    return false;
  }

  // find first continues \n position
  size_t pos = content.find("\n\n");
  if (pos == string::npos) {
    cout << RED << "Invalid file format!\n" << RESET_COLOR;
    return false;
  }

  string serialized_player_board = content.substr(0, pos);
  string serialized_computer_board = content.substr(pos + 2);

  Board playerBoard = Board::deserialize(serialized_player_board);
  this->playerBoard = playerBoard;

  Board computerBoard = Board::deserialize(serialized_computer_board);
  this->computerBoard = computerBoard;

  fin.close();
  this->initialized = true;
  cout << "Game loaded" << GREEN << " successfully!\n" << RESET_COLOR;

  return true;
}

void Game::saveGame() {
  cout << "Saving game...\n";
  ofstream fout;
  fout.open("savegame.txt");
  fout << playerBoard.serialize();

  fout << "\n\n";
  fout << computerBoard.serialize();
  fout.close();

  cout << "Game saved" << GREEN << " successfully!\n" << RESET_COLOR;
  cout << "Enter any key to continue...";
  cout.flush();
  cin.get();
}

bool Game::playerMove() {
  int x = -1;
  int y = -1;
  if (!gameLogic.getMoveFromPlayer(playerBoard, computerBoard, x, y)) {
    return false;
  }

  if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
    if (computerBoard.handleHit(x, y)) {
      cout << "Hit!\n";
    } else {
      cout << "Miss!\n";
    }
  } else {
    cout << RED << "Invalid coordinates!\n" << RESET_COLOR;
  }
  return true;
}

void Game::computerMove() {
  while (true) {
    Point p = playerBoard.getRandomPoint();
    if (!playerBoard.isHit(p.x, p.y)) {
      if (playerBoard.handleHit(p.x, p.y)) {
        cout << "Computer's turn: Hit at " << char('A' + p.x) << p.y << endl;
      } else {
        cout << "Computer's turn: Miss at " << char('A' + p.x) << p.y << endl;
      }
      break;
    }
  }
}

Game::Game() : playerTurn(true), initialized(false) { srand(time(0)); }

Game::~Game() {}

void Game::start() {
  if (!initialized) {
    clearScreen();
    placeShips(playerBoard, true);
    placeShips(computerBoard, false);
    clearScreen();
  }

  while (true) {
    if (playerTurn) {
      if (!playerMove()) {
        saveGame(); // 中途退出游戏，保存游戏存档
        return;
      }
      if (computerBoard.allShipsSunk()) {
        displayBoardsSideBySide(playerBoard, computerBoard, true, -1, -1, true);
        cout << YELLOW << "Congratulations! You win!\n" << RESET_COLOR;
        cout << "Enter any key to continue...";
        cin.get();
        break;
      }

    } else {
      computerMove();
      if (playerBoard.allShipsSunk()) {
        displayBoardsSideBySide(playerBoard, computerBoard, true, -1, -1, true);
        cout << RED << "Sorry, computer wins!\n" << RESET_COLOR;
        cout << "Enter any key to continue...";
        cin.get();
        break;
      }
    }
    playerTurn = !playerTurn;
  }
  saveGame(); // 保存游戏结局
}