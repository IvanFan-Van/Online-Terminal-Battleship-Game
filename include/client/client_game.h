#ifndef CLIENTGAME_H
#define CLIENTGAME_H

#include "battleship/board.h"
#include "client/ctcpclient.h"
#include "common/game_logic.h"
#include <string>
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
  void init(Board &board);

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

  void handleMessage(const string &message);
};

#endif // CLIENTGAME_H