#include "common/game_action.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

int main() {
  int client_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (client_fd == -1) {
    cout << "Failed to create socket\n";
    return 1;
  }

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(3004);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  if (connect(client_fd, (struct sockaddr *)&server_addr,
              sizeof(server_addr)) == -1) {
    cout << "Failed to connect to server\n";
    return 1;
  }

  cout << "Connected to server\n";

  // 发送匹配码
  string matchCode = "1111";
  GameAction action(ActionType::JOIN, matchCode);
  string actionStr = action.serialize();
  if (send(client_fd, actionStr.c_str(), actionStr.size(), 0) == -1) {
    cout << "Failed to send match code\n";
    return 1;
  }
  cout << "Sent match code successfully\n";

  // 接收匹配结果
  char buffer[1024];
  memset(buffer, 0, sizeof(buffer));
  ssize_t bytes = recv(client_fd, buffer, sizeof(buffer), 0);
  if (bytes == -1) {
    cout << "Failed to receive match result\n";
    return 1;
  }
  string matchResult(buffer, bytes);
  cout << "Received match result: " << matchResult << endl;
  if (matchResult != "Match Success") {
    cout << "Failed to match\n";
    cout << "Waiting for match...\n";
  }

  while (matchResult != "Match Success") {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    bytes = recv(client_fd, buffer, sizeof(buffer), 0);
    if (bytes == -1) {
      cout << "Failed to receive match result\n";
      return 1;
    }
    if (bytes == 0) {
      cout << "recv returned 0\n";
    }
    cout << "Received buffer: " << buffer << "\n";
    matchResult.assign(buffer, bytes);
    cout << "Received match result: " << matchResult << endl;

    if (matchResult == "Match Success") {
      cout << "Match found\n";
      break;
    }
  }

  cout << "Match Success\n";

  return 0;
}