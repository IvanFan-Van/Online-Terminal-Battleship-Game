#include "client/ctcpclient.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

// 初始化行为结构体P
struct InitAction {
  char board[10][10];
};

// 攻击行为结构体
struct AttackAction {
  int x;
  int y;
};

/**
 * 连接服务器, 连接成功返回`true`, 否则返回`false`
 */
bool CTcpClient::connect(const string &server_ip, const int server_port) {
  if (socket_fd != -1) {
    cout << "already connected\n";
    return false;
  }

  this->server_ip = server_ip;
  this->server_port = server_port;
  // 1. create socket
  if ((socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
    perror("socket failed to create");
    return false;
  };
  cout << "socket created successfully\n";

  // 2. connect to server
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(server_port);
  server_addr.sin_addr.s_addr = inet_addr(server_ip.c_str());
  if (::connect(socket_fd, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) == -1) {
    perror("connect failed");
    return false;
  }
  cout << "connected to server\n";

  return true;
}

bool CTcpClient::send(const string &buffer) {
  if (socket_fd == -1) {
    perror("socket not connected");
    return false;
  }

  if (::send(socket_fd, buffer.data(), buffer.size(), 0) == -1) {
    perror("send failed");
    return false;
  }

  return true;
}

bool CTcpClient::send(const void *buffer, const size_t len) {
  if (socket_fd == -1) {
    perror("socket not connected");
    return false;
  }

  if (::send(socket_fd, buffer, len, 0) == -1) {
    perror("send failed");
    return false;
  }

  return true;
}

bool CTcpClient::recv(string &buffer, const size_t maxlen) {
  if (socket_fd == -1) {
    perror("socket not connected");
    return false;
  }

  buffer.clear();
  buffer.resize(maxlen);
  int iret = ::recv(socket_fd, &buffer[0], maxlen, 0);
  if (iret <= 0) {
    perror("read failed");
    buffer.clear();
    return false;
  }

  buffer.resize(iret);
  return true;
}

bool CTcpClient::recv(void *buffer, const size_t len) {
  if (socket_fd == -1) {
    perror("socket not connected");
    return false;
  }

  int iret = ::recv(socket_fd, buffer, len, 0);
  if (iret <= 0) {
    perror("read failed");
    return false;
  }

  return true;
}

bool CTcpClient::close() {
  if (socket_fd == -1) {
    perror("socket not connected");
    return false;
  }

  ::close(socket_fd);
  socket_fd = -1;
  return true;
}

CTcpClient::CTcpClient() { socket_fd = -1; }
CTcpClient::~CTcpClient() { close(); }

int CTcpClient::getSocketFd() const { return socket_fd; }
