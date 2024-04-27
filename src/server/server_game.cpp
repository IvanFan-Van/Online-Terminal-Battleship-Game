#include "server/server_game.h"
#include "client/ctcpclient.h"
#include "common/action.h"
#include "common/constants.h"
// #include "common/serialization.h"
#include "common/utilities.h"
#include <iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

/**
 * 构造函数
 */
ServerGame::ServerGame(int fd1, int fd2) {
  Board board1;
  Board board2;
  boards.emplace(fd1, board1);
  boards.emplace(fd2, board2);
  player1 = fd1;
  player2 = fd2;
  currentPlayer = player1;
  playerTurns.emplace(fd1, true);
  playerTurns.emplace(fd2, false);
}

/**
 * 析构函数
 */
ServerGame::~ServerGame() {
  if (player1 != -1) {
    close(player1);
  }
  if (player2 != -1) {
    close(player2);
  }
}

// 行为处理调度器
void ServerGame::handlePlayerAction(const GameAction &action, int client_fd) {

  switch (action.type) {
  case INIT: {
    handleInitAction(action, client_fd);
    break;
  }
  case START: {
    handleStartAction(action, client_fd);
    break;
  }
  case SHOOT: {
    handleShootAction(action, client_fd);
    // switchTurn();
    break;
  }
  case CHECK_WIN: {
    handleCheckWinAction(action, client_fd);
    break;
  }
  case GET_GAME_STATUS: {
    handleGetGameStatusAction(action, client_fd);
    break;
  }
  default: {
    cout << "Invalid action type from client " << client_fd << ": "
         << action.type << endl;
    break;
  }
  }
}

/**
 * 处理初始化行为
 */
void ServerGame::handleInitAction(const GameAction &action, int client_fd) {
  cout << "Received init action from client " << client_fd << endl;
  // 从客户端接收棋盘信息, 并初始化棋盘
  cout << "Place ships for player " << client_fd << endl;
  // 从客户端接收棋盘信息
  Board board = Board::deserialize(action.data);
  boards[client_fd] = board;
  cout << "player " << client_fd << " initialized the board successfully"
       << endl;
}
/**
 * 处理开始游戏行为
 */
void ServerGame::handleStartAction(const GameAction &action, int fd) {
  cout << "Received start action from client " << fd << endl;

  // 检查是否双方都初始化完毕
  bool fullyInitialized;
  if (!this->fullyInitialized()) {
    string info = "Waiting for the other player to initialize the board...";
    cout << info << endl;

    fullyInitialized = false;
    if (send(fd, &fullyInitialized, sizeof(bool), 0) == -1) {
      cout << "Failed to send initialize status to client " << fd << endl;
    };
  } else {
    string info = "Both players have initialized the board. Game starts!";
    cout << info << endl;

    fullyInitialized = true;
    if (send(fd, &fullyInitialized, sizeof(bool), 0) == -1) {
      cout << "Failed to send initialize status to client " << fd << endl;
    };

    // 通知玩家回合
    this->notifyPlayerTurn(fd);
  }
}
/**
 * 处理攻击行为
 */
void ServerGame::handleShootAction(const GameAction &action, int client_fd) {
  Point point = Point::deserialize(action.data);
  cout << "Received shoot action from client " << client_fd << ": " << point.x
       << ", " << point.y << endl;

  // 检查是否是当前玩家回合
  if (client_fd != currentPlayer) {
    cout << "Not player " << client_fd << "'s turn" << endl;
    return;
  }

  int x = point.x;
  int y = point.y;
  int opponent_fd = client_fd == player1 ? player2 : player1;
  Board &opponentBoard = boards[opponent_fd];
  bool hit = opponentBoard.handleHit(x, y);
  if (send(client_fd, &hit, sizeof(bool), 0) == -1) {
    cout << "Failed to send hit status to client " << client_fd << endl;
  };
}
/**
 * 处理检查胜利行为
 */
void ServerGame::handleCheckWinAction(const GameAction &action, int client_fd) {
  cout << "Received check win action from client " << client_fd << endl;

  string message;
  int opponent_fd = client_fd == player1 ? player2 : player1;
  if (boards[opponent_fd].allShipsSunk()) {
    cout << "winner is " << client_fd << endl;
    message = "You win";
    if (send(client_fd, message.c_str(), message.size(), 0) == -1) {
      cout << "Failed to send win status to client " << client_fd << endl;
    };
    message = "You lose";
    if (send(opponent_fd, message.c_str(), message.size(), 0) == -1) {
      cout << "Failed to send win status to client " << opponent_fd << endl;
    };
  } else if (boards[client_fd].allShipsSunk()) {
    cout << "winner is " << opponent_fd << endl;
    message = "You lose";
    if (send(client_fd, message.c_str(), message.size(), 0) == -1) {
      cout << "Failed to send win status to client " << client_fd << endl;
    };
    message = "You win";
    if (send(opponent_fd, message.c_str(), message.size(), 0) == -1) {
      cout << "Failed to send win status to client " << opponent_fd << endl;
    };
  } else {
    cout << "winner haven't been decided yet" << endl;
    message = "Game continues";
    if (send(client_fd, message.c_str(), message.size(), 0) == -1) {
      cout << "Failed to send win status to client " << endl;
    };

    switchTurn();
  }
}
/**
 * 处理获取游戏状态行为
 */
void ServerGame::handleGetGameStatusAction(const GameAction &action,
                                           int client_fd) {
  cout << "Received get game status action from client " << client_fd << endl;

  // 向客户端发送自己棋盘信息
  Board &playerBoard = boards[client_fd];
  string serializedPlayerBoard = playerBoard.serialize();
  cout << "展示序列化后的玩家棋盘信息: \n" << serializedPlayerBoard << endl;
  if (send(client_fd, serializedPlayerBoard.c_str(),
           serializedPlayerBoard.size(), 0) == -1) {
    cout << "Failed to send player board to client " << client_fd << endl;
  };

  // 从客户端接收确认信息
  char buffer[1024];
  if (recv(client_fd, buffer, sizeof(buffer), 0) == -1) {
    cout << "Failed to receive confirmation from client " << client_fd << endl;
  };
  if (strcmp(buffer, "OK") != 0) {
    cout << "Failed to receive confirmation from client " << client_fd << endl;
  };

  // 向客户端发送对手棋盘信息
  int opponent_fd = client_fd == player1 ? player2 : player1;
  Board &opponentBoard = boards[opponent_fd];
  string serializedOpponentBoard = opponentBoard.serialize();
  cout << "展示序列化后的对手棋盘信息: \n" << serializedOpponentBoard << endl;
  if (send(client_fd, serializedOpponentBoard.c_str(),
           serializedOpponentBoard.size(), 0) == -1) {
    cout << "Failed to send opponent board to client " << client_fd << endl;
  };
  cout << "序列化信息发送成功" << endl;
}

/**
 * 通知玩家回合
 */
void ServerGame::notifyPlayerTurn(int fd) {
  if (playerTurns[fd]) {
    const char *message = "Your turn";
    send(fd, message, strlen(message), 0);
  } else {
    const char *message = "Opponent's turn";
    send(fd, message, strlen(message), 0);
  }
}

/**
 * 切换回合
 */
void ServerGame::switchTurn() {
  currentPlayer = currentPlayer == player1 ? player2 : player1;

  // 更新玩家回合
  for (auto &turn : playerTurns) {
    turn.second = !turn.second;
  }

  // 通知当前玩家回合
  notifyPlayerTurn(currentPlayer);
}

/**
 * 判断是否初始化完毕
 */
bool ServerGame::fullyInitialized() {
  return boards[player1].allShipsPlaced() && boards[player2].allShipsPlaced();
}
