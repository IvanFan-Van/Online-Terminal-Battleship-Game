#include "server/cepollserver.h"
#include "common/action.h"
#include <arpa/inet.h>
#include <functional>
#include <iostream>
#include <string.h>
#include <string>
#include <sys/epoll.h>
#include <unistd.h>
#include <unordered_map>

using namespace std;
// 构造器
CEpollServer::CEpollServer() : server_fd(-1), epoll_fd(-1), events(nullptr) {}
CEpollServer::~CEpollServer() { stop(); }

// 生命周期函数
bool CEpollServer::start(const string &ip, int port, int max_events) {
  this->max_events = max_events;
  // Create server socket and epoll instance
  if (!createServerSocket(ip, port) || !createEpollInstance()) {
    cout << "Failed to create server socket or create epoll instance" << endl;
    return false;
  }
  events = new epoll_event[max_events];
  return true;
}

void CEpollServer::run() {
  while (true) {
    int nfds = epoll_wait(epoll_fd, events, max_events,
                          -1); // Wait for events to occur
    if (nfds == -1) {
      cout << "epoll_wait failed" << endl;
      break;
    }
    for (int i = 0; i < nfds; i++) {
      if (events[i].data.fd == server_fd) {
        cout << "New client connection" << endl;
        handleNewConnection(); // Handle new client connection
      } else {
        cout << "Client message received" << endl;
        handleClientMessage(events[i].data.fd); // Handle client message
      }
    }
  }

  stop();
}

void CEpollServer::stop() {
  if (server_fd != -1) {
    closeFd(server_fd); // Close server socket
    server_fd = -1;
  }
  if (epoll_fd != -1) {
    closeFd(epoll_fd); // Close epoll instance
    epoll_fd = -1;
  }
  if (events != nullptr) {
    delete[] events; // Deallocate events array
    events = nullptr;
  }
}

// 设置回调函数
void CEpollServer::setOnClientConnected(function<void(int)> callback) {
  onClientConnected = callback; // Set callback for client connection event
}
void CEpollServer::setOnClientDisconnected(function<void(int)> callback) {
  onClientDisconnected =
      callback; // Set callback for client disconnection event
}
void CEpollServer::setOnMessageReceived(
    function<void(int, void *buffer, int bytes)> callback) {
  onMessageReceived = callback; // Set callback for message received event
}

// 创建服务器套接字
bool CEpollServer::createServerSocket(const string &ip, int port) {
  server_fd = socket(AF_INET, SOCK_STREAM, 0); // Create TCP socket
  if (server_fd == -1) {
    perror("Failed to create server socket"); // Print error message
    return false;
  }

  // TODO only for development mode
  // Set SO_REUSEADDR to allow immediate reuse of the port
  int opt = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) ==
      -1) {
    cout << "Failed to set SO_REUSEADDR option" << endl;
    close(server_fd);
    return false;
  }

  // Bind server socket to IP and port
  sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);
  if (bind(server_fd, (sockaddr *)&addr, sizeof(addr)) == -1) {
    cout << "Failed to bind server socket" << endl;
    return false;
  }

  // Listen for incoming connections
  // SOMAXCONN is a global variable that defines the maximum length
  if (listen(server_fd, SOMAXCONN) == -1) {
    // for the queue of pending connections, default is 128
    cout << "Failed to listen on server socket" << endl;
    return false;
  }

  return true;
}

// Create epoll instance
bool CEpollServer::createEpollInstance() {
  epoll_fd = epoll_create1(0); // Create epoll instance
  if (epoll_fd == -1) {
    cout << "Failed to create epoll instance" << endl;
    return false;
  }
  // Add server socket to epoll instance
  return addFdToEpoll(server_fd, EPOLLIN);
}

// Add file descriptor to epoll instance
bool CEpollServer::addFdToEpoll(int fd, uint32_t events) {
  epoll_event event;
  event.events = events;
  event.data.fd = fd;
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1) {
    cout << "Failed to add file descriptor to epoll instance" << endl;
    return false;
  }
  return true;
}

// Handle new client connection
bool CEpollServer::handleNewConnection() {
  sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);
  int client_fd = accept(server_fd, (sockaddr *)&addr,
                         &addr_len); // Accept client connection
  if (client_fd == -1) {
    cout << "Failed to accept client connection" << endl;
    return false;
  }
  if (!addFdToEpoll(client_fd, EPOLLIN)) {
    cout << "Failed to add client socket to epoll instance" << endl;
    return false;
  }
  callbacks[client_fd] = [this, client_fd]() {
    if (onClientConnected) {
      onClientConnected(client_fd); // Call client connection event callback
    }
  };
  callbacks[client_fd](); // Call client connection event callback
  return true;
}

// Handle client message
bool CEpollServer::handleClientMessage(int client_fd) {
  char buffer[1024];
  memset(buffer, 0, sizeof(buffer));
  int bytes =
      recv(client_fd, buffer, sizeof(buffer), 0); // Receive message from client
  if (bytes <= 0) {
    if (bytes == 0) {
      cout << "Client disconnected" << endl;
    } else {
      cout << "Failed to receive message from client" << endl;
    }
    if (onClientDisconnected) {
      onClientDisconnected(
          client_fd); // Call client disconnection event callback
    }
    closeFd(client_fd); // Close client socket
    return false;
  } else {
    if (onMessageReceived) {
      onMessageReceived(client_fd, buffer,
                        bytes); // Call message received event callback
    }
  }
  return true;
}

// Close file descriptor
void CEpollServer::closeFd(int fd) {
  cout << "Closing file descriptor: " << fd << endl;
  close(fd);
}

void CEpollServer::removeFd(int fd) {
  epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, fd, NULL);
}