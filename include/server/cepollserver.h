#ifndef CEPOLLSERVER_H
#define CEPOLLSERVER_H

#include <sys/epoll.h>
#include <unordered_map>
#include <functional>
#include <string>
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <atomic>
#include <map>
#include <vector>
#include <mutex>

using namespace std;

class CEpollServer
{
private:
  int server_fd;                                      // Server socket file descriptor
  int epoll_fd;                                       // Epoll instance file descriptor
  int max_events;                                     // Maximum number of events to handle
  struct epoll_event *events;                         // Array of epoll events
  unordered_map<int, function<void()>> callbacks;     // Map of callbacks for each file descriptor
  function<void(int)> onClientConnected;              // Callback for client connection event
  function<void(int)> onClientDisconnected;           // Callback for client disconnection event
  function<void(int, void *, int)> onMessageReceived; // Callback for message received event
  // 用于存储等待匹配的连接
  map<string, vector<int>> waiting_queue;
  mutex queue_mutex;

  // Create server socket
  bool createServerSocket(const string &ip, int port);

  // Create epoll instance
  bool createEpollInstance();

  // Add file descriptor to epoll instance
  bool addFdToEpoll(int fd, uint32_t events);

  // Handle new client connection
  bool handleNewConnection();

  // Handle client message
  bool handleClientMessage(int client_fd);

  // Close file descriptor
  void closeFd(int fd);

public:
  CEpollServer();
  ~CEpollServer();

  bool start(const string &ip, int port, int max_events = 10);

  void run();

  void stop();

  void removeFd(int fd);

  void setOnClientConnected(function<void(int)> callback);
  void setOnClientDisconnected(function<void(int)> callback);
  void setOnMessageReceived(function<void(int, void *buffer, int bytes)> callback);
};

#endif // CEPOLLSERVER_H