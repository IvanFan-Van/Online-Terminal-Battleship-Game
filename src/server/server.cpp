#include "common/action.h"
#include "common/constants.h"
#include "server/cepollserver.h"
#include "server/server_game.h"
#include <cstring>
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <string>
#include <sys/epoll.h>
#include <thread>
#include <unordered_map>

using namespace std;

unordered_map<int, string> clientMatchCodes; // 用于存储客户端匹配码
unordered_map<string, int> waitingClients; // 用于存储等待匹配的客户端
CEpollServer server;                       // Epoll 服务器封装

void handleMatchedClients(int client1_fd, int client2_fd) {
  // 创建epoll监听事件
  int epoll_fd = epoll_create1(0);
  epoll_event events[2]; // 最多有两个监听事件
  epoll_event event;     // 创建epoll事件
  event.events = EPOLLIN;
  // 添加第一个客户端socket
  event.data.fd = client1_fd;
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client1_fd, &event);
  // 添加第二个客户端socket
  event.data.fd = client2_fd;
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client2_fd, &event);

  ServerGame game = ServerGame(client1_fd, client2_fd);
  while (true) {
    int nfds = epoll_wait(epoll_fd, events, 2, -1);
    if (nfds == -1) {
      cout << "epoll_wait failed" << endl;
      break;
    }
    for (int i = 0; i < nfds; i++) {
      if (events[i].events & EPOLLIN) {
        // 获取激活的文件描述符
        int activate_fd = events[i].data.fd;

        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        // 接收数据
        ssize_t bytes_received = recv(activate_fd, buffer, sizeof(buffer), 0);

        // epoll_wait error
        if (bytes_received == -1) {
          cout << "Failed to receive data from client " << activate_fd << endl;
          break;
          // 客户端退出
        } else if (bytes_received == 0) {
          cout << "Client " << activate_fd << " disconnected" << endl;
          // 当其中一方退出时关闭所有连接
          close(client1_fd);
          close(client2_fd);

          // 从 epoll 实例中移除文件描述符
          epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client1_fd, nullptr);
          epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client2_fd, nullptr);
          break;
        }

        // 接收GameAction
        string receivedData;
        receivedData.assign(buffer, bytes_received);
        GameAction action = GameAction::deserialize(receivedData);
        cout << "Received Action from client " << activate_fd << ": "
             << action.type << " " << action.data << endl;

        game.handlePlayerAction(action, activate_fd);
      } else {
        cout << "epoll_wait not EPOLLIN" << endl;
        break;
      }
    }
  }
}

void onClientConnected(int client_fd) {
  cout << "Client connected with file descriptor: " << client_fd << endl;
}

void onClientDisconnected(int client_fd) {
  cout << "Client disconnected with file descriptor: " << client_fd << endl;
  // 从等待匹配队列中移除
  string matchCode = clientMatchCodes[client_fd];
  if (!matchCode.empty() && waitingClients.count(matchCode) &&
      waitingClients[matchCode] == client_fd) {
    waitingClients.erase(matchCode);
  }
  clientMatchCodes.erase(client_fd);
}

void onMessageReceived(int client_fd, void *buffer, int bytes) {
  // 存储或匹配客户端
  if (bytes == 4) { // 假设匹配码是4个字符
    char *message = reinterpret_cast<char *>(buffer);
    cout << "Received match code from client " << client_fd << ": " << message
         << endl;
    clientMatchCodes[client_fd] = message;
    if (waitingClients.count(message)) {
      // 找到匹配，开始配对通信
      int waitingClientFd = waitingClients[message];
      // 这里可以启动新的线程或逻辑来处理配对的两个客户端...
      cout << "Match found between " << client_fd << " and " << waitingClientFd
           << endl;
      thread(handleMatchedClients, client_fd, waitingClientFd).detach();
      // 假设我们发送一个简单的匹配成功消息
      string matchSuccess = "Match Success";
      send(client_fd, matchSuccess.c_str(), matchSuccess.size(), 0);
      send(waitingClientFd, matchSuccess.c_str(), matchSuccess.size(), 0);
      // 删除主线程对以配对socket的监听, 转而让子线程处理监听
      server.removeFd(client_fd);
      server.removeFd(waitingClientFd);
      // 清理等待列表
      waitingClients.erase(message);
    } else {
      cout << "No match found for client " << client_fd << " Waiting..."
           << endl;
      // 没有找到匹配，加入等待队列
      waitingClients[message] = client_fd;
      string waitingMsg = "Waiting for match...";
      send(client_fd, waitingMsg.c_str(), waitingMsg.size(), 0);
    }
  } else {
    // 不是有效的匹配码或 Action 结构体消息
    string info =
        "Invalid message format received from client " + to_string(client_fd);
    cout << info << endl;
    send(client_fd, info.c_str(), info.length(), 0);
  }
}

void handleSignal(int signal) {
  cout << "Received signal: " << signal << endl;
  server.stop();
}

int main() {
  signal(SIGINT, handleSignal);

  server.setOnClientConnected(onClientConnected);
  server.setOnClientDisconnected(onClientDisconnected);
  server.setOnMessageReceived(onMessageReceived);

  if (!server.start("43.143.114.119", 3004)) {
    cerr << "Failed to start server!" << endl;
    return -1;
  }

  server.run(); // 运行服务器的事件循环

  return 0;
}
