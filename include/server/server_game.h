#ifndef SERVERGAME_H
#define SERVERGAME_H

#include "battleship/board.h"
#include "common/game_action.h"
#include <string>
#include <unordered_map>

using namespace std;

class ServerGame {
private:
  // mapping client_fd -> board
  unordered_map<int, Board> boards;
  // A flag indicating whether it is the player's turn.
  int currentPlayer;
  // mapping from player fd to bool indicating whether it is the player's turn
  unordered_map<int, bool> playerTurns;

  int player1;
  int player2;

  void handleInitAction(const GameAction &action, int client_fd);

  void handleStartAction(const GameAction &action, int fd);

  void handleShootAction(const GameAction &action, int client_fd);

  void handleCheckWinAction(const GameAction &action, int client_fd);

  void handleGetGameStatusAction(const GameAction &action, int client_fd);

  bool fullyInitialized();

public:
  /**
   * @brief Constructor for online playing
   */
  ServerGame(int fd1, int fd2);

  ServerGame();

  ~ServerGame();

  // 行为处理调度器
  void handlePlayerAction(const GameAction &action, int fd);

  /**
   * @brief Starts the game.
   *
   * This function starts the game by initializing the boards, placing the
   * ships, and executing the turns until the game is over.
   */
  void start();

  void notifyPlayerTurn(int client_fd);

  void switchTurn();

  void stop();
};

#endif // SERVERGAME_H