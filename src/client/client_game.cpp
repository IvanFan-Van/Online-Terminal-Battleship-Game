#include "client/client_game.h"

#include <thread>

using namespace std;

void ClientGame::init() {
  // 初始化船只并往board上放置
  for (int shipSize : SHIPS) {
    Ship ship(shipSize);
    gameLogic.placeShips(playerBoard, ship);
  }

  // 发送初始化棋盘行为
  playerBoard.display(true);
  // cout << "size of GameAction: " << sizeof(GameAction) << endl;
  // 初始化棋盘
  GameAction initAction(INIT, playerBoard.serialize());
  string data = initAction.serialize();
  // cout << "size of initAction: " << sizeof(initAction) << endl;

  // TODO
  if (!client.send(data.c_str(), data.size())) {
    cout << "Failed to send board to server\n";
    return;
  };
}

void ClientGame::getGameStatus() {
  GameAction action(GET_GAME_STATUS);
  string data = action.serialize();
  if (!client.send(data.c_str(), data.size())) {
    cout << "Failed to send get game status action to server\n";
    return;
  };

  // 公用缓冲区
  char buffer[1024];
  memset(buffer, 0, sizeof(buffer));

  // 接收自己 Board
  Board playerBoard;
  string receivedData1;
  ssize_t bytes = recv(client.getSocketFd(), buffer, sizeof(buffer), 0);
  if (bytes == -1) {
    cout << "Failed to receive player board from server\n";
    return;
  };
  receivedData1.assign(buffer, bytes);
  // cout << "Successfully received player board from server\n";
  this->playerBoard = Board::deserialize(receivedData1);

  if (!client.send("OK", 2)) {
    cout << "Failed to send OK to server\n";
  };

  // 接收对手 Board
  Board opponentBoard;
  memset(buffer, 0, sizeof(buffer));
  bytes = recv(client.getSocketFd(), buffer, sizeof(buffer), 0);
  if (bytes == -1) {
    cout << "Failed to receive opponent board from server\n";
    return;
  };

  string receivedData2;
  receivedData2.assign(buffer, bytes);

  opponentBoard = Board::deserialize(receivedData2);
  this->opponentBoard = opponentBoard;

  cout << "Successfully received game status from server\n";
}

void ClientGame::playerMove() {
  getGameStatus();

  int x = -1;
  int y = -1;
  gameLogic.getMoveFromPlayer(playerBoard, opponentBoard, x, y);

  GameAction action(SHOOT, Point(x, y).serialize());
  string data = action.serialize();
  if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {

    if (!client.send(data.c_str(), data.size())) {
      cout << "Failed to send shoot action to server\n";
      return;
    };

    bool hit;
    if (!client.recv(&hit, sizeof(bool))) {
      cout << "Failed to receive hit status from server\n";
      return;
    };

    if (hit) {
      cout << "Hit!\n";
    } else {
      cout << "Miss!\n";
    }

    getGameStatus();
    displayBoardsSideBySide(playerBoard, opponentBoard, true, -1, -1);
    cout << "Waiting for the opponent to move...\n";
    checkWin();
  } else {
    cout << "Invalid coordinates!\n";
  }
}

ClientGame::ClientGame(string &ip) : gameStarted(false) {
  this->ip = ip;
  srand(time(0));
  this->playerBoard = Board();
  // 连接服务器
  // 创建客户端对象
  this->client = CTcpClient();
}

ClientGame::~ClientGame() {
  cout << "Game Exit...\n";
  client.close();
}

void ClientGame::checkWin() {
  GameAction action(CHECK_WIN);
  string data = action.serialize();
  if (!client.send(data.c_str(), data.size())) {
    cout << "Failed to send check win action to server\n";
    return;
  };

  string message;
  if (!client.recv(message, 1024)) {
    cout << "Failed to receive message from server\n";
    return;
  };
  handleMessage(message);
  return;
}

void ClientGame::checkStart() {
  GameAction action(START);
  string data = action.serialize();
  if (!client.send(data.c_str(), data.size())) {
    cout << "Failed to send start action to server\n";
    return;
  };

  if (!client.recv(&gameStarted, sizeof(bool))) {
    cout << "Failed to receive game start status from server\n";
    return;
  };
}

void ClientGame::stop() { client.close(); }

string trim(const string &str) {
  const auto strBegin = str.find_first_not_of(" \t\r\n\0");
  if (strBegin == string::npos) {
    return "";
  }

  const auto strEnd = str.find_last_not_of(" \t\r\n\0");
  const auto strRange = strEnd - strBegin + 1;

  return str.substr(strBegin, strRange);
}

void ClientGame::handleLost() {
  getGameStatus();
  displayBoardsSideBySide(playerBoard, opponentBoard, true, -1, -1, true);
  cout << RED << "Sorry, You Lose\n" << RESET_COLOR;
  stop();
}

void ClientGame::handleMessage(const string &rawMessage) {
  string message = trim(rawMessage);
  if (message == "Your turn") {
    cout << "Your turn.\n";
    playerMove();
  } else if (message == "Opponent's turn") {
    cout << "Opponent's turn.\n";
  } else if (message == "You win") {
    cout << YELLOW << "Congratulations! You win!\n" << RESET_COLOR;
    stop();
  } else if (message == "You lose") {
    handleLost();
  }
}

void ClientGame::waitGameStart() {
  // 等待游戏开始
  if (!gameStarted) {
    cout << "Waiting for the other player to initialize the board...\n";
  }
  while (!gameStarted) {
    checkStart();
    sleep(1);
  }
  cout << "Game started!\n";
}

void ClientGame::joinGame() {
  // 获取匹配码
  cout << "Enter Match Code: ";
  string match_code;
  getline(cin, match_code);
  while (!regex_match(match_code, regex("\\d{4}"))) {
    cout << RED << "Invalid match code. Please enter a 4-digit number: "
         << RESET_COLOR;
    getline(cin, match_code);
  }

  GameAction action;
  action.type = JOIN;
  action.data = match_code;
  string data = action.serialize();
  if (!client.send(data.c_str(), data.size())) {
    cout << "send failed\n";
    stop();
  }

  // 等待匹配成功
  char buffer[1024];
  memset(buffer, 0, sizeof(buffer));
  int valread = recv(client.getSocketFd(), buffer, sizeof(buffer), 0);
  if (valread < 0) {
    cout << "recv failed\n";
    stop();
  }
  cout << buffer << endl;

  string receivedData;
  receivedData.assign(buffer, valread);
  while (receivedData != "Match Success") {
    memset(buffer, 0, sizeof(buffer));
    int valread = recv(client.getSocketFd(), buffer, sizeof(buffer), 0);
    cout << "阻塞失败..." << endl;
    if (valread < 0) {
      cout << "recv failed\n";
      break;
    } else if (valread == 0) {
      cout << "Connection closed by server\n";
    }

    receivedData.assign(buffer, valread);
    cout << "Received: " << buffer << "\n";
    if (receivedData == "Match Success") {
      cout << "Match Success\n";
      break;
    }
  }
}

void ClientGame::start() {
  // 连接服务器
  if (!client.connect(ip, 3004)) {
    cout << "connect failed\n";
    stop();
  };
  // 加入游戏
  joinGame();
  // 初始化棋盘
  init();
  // 等待游戏开始
  waitGameStart();
  while (true) {
    string message;
    if (!client.recv(message, 1024)) {
      cout << "Failed to receive message from server\n";
      break;
    };
    handleMessage(message);
  }
}