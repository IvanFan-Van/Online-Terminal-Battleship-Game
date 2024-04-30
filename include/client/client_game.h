#ifndef CLIENTGAME_H
#define CLIENTGAME_H

#include "battleship/board.h"
#include "battleship/game.h"
#include "client/ctcpclient.h"
#include "common/action.h"
#include "common/constants.h"
#include "common/game_action.h"
#include "common/utilities.h"

#include <algorithm>
#include <iostream>
#include <regex>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <unordered_map>

using namespace std;

class ClientGame {
private:
  CTcpClient client;   // The client object used to communicate with the server.
  Board playerBoard;   // The board representing the player's ships and attacks.
  Board opponentBoard; // Only the hits from the opponent
  bool gameStarted;    // A flag indicating whether the game has started.
  string ip;
  GameLogic gameLogic; // An instance of the GameLogic class.
  /**
   * @brief Places the ships on the given board.
   *
   * This function is used to place the ships on the board for either the player
   * or the computer.
   *
   * @param board The board on which to place the ships.
   */
  void init();

  /**
   * @brief Executes the player's move.
   *
   * This function is called when it is the player's turn to make a move.
   */
  void playerMove();

  void getGameStatus();

  void checkStart();

  /**
   * @brief Executes the computer's move.
   *
   * This function is called when it is the computer's turn to make a move.
   */
  void computerMove();

  /**
   * @brief Check if player wins
   */
  void checkWin();

  void handleLost();

  void handleMessage(const string &message);

  void waitGameStart();

  void joinGame();

public:
  /**
   * @brief Default constructor for the Game class.
   */
  ClientGame();

  ClientGame(string &ip);

  ~ClientGame();

  /**
   * @brief Starts the game.
   *
   * This function starts the game by initializing the boards, placing the
   * ships, and executing the turns until the game is over.
   */
  void start();

  void stop();
};

#endif // CLIENTGAME_H