
#include "common/action.h"
#include "server/server_game.h"

#include <errno.h>
#include <iostream>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <unordered_map>

#include <ctime>
#include <random>

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>

using namespace std;

unordered_map<string, int> waiting_list;             // 等待队列
unordered_map<int, unsigned long> client_game_table; // 客户端和游戏会话ID的映射

static const int PORT = 3004;
static const int MAX_CONNECTIONS = 1024;

// 会话管理器
class SessionManager {
public:
  unordered_map<unsigned long, ServerGame> games;
  SessionManager(){};

  unsigned long create_session(int client1, int client2) {
    unsigned long session_id = generate_session_id();
    ServerGame game(client1, client2);
    games[session_id] = game;
    return session_id;
  }

  ServerGame &get_game(unsigned long session_id) { return games[session_id]; }

private:
  unsigned long generate_session_id() {
    // 随机数生成器
    static random_device rd;
    // 生成器
    static mt19937_64 generator(rd());
    uniform_int_distribution<unsigned long> distribution;

    unsigned long session_id = 0;
    do {
      session_id = distribution(generator);
    } while (games.count(session_id) > 0);

    return session_id;
  }
};

// 线程池
class ThreadPool {
private:
  atomic<bool> stop;             // 是否停止
  vector<thread> workers;        // 线程
  queue<function<void()>> tasks; // 任务队列

  mutex queue_mutex;            // 任务队列锁
  condition_variable condition; // 条件变量

public:
  ThreadPool(size_t num_threads) : stop(false) {
    for (size_t i = 0; i < num_threads; i++) {
      workers.emplace_back([this] {
        while (true) {
          function<void()> task;
          {
            // 获取任务
            unique_lock<mutex> lock(this->queue_mutex);
            this->condition.wait(lock, [this] {
              // 等待任务队列不为空或者停止
              return this->stop.load() || !this->tasks.empty();
            });
            if (this->stop.load() && this->tasks.empty()) {
              return;
            }
            task = move(this->tasks.front());
            this->tasks.pop();
          }
          task();
        }
      });
    }
  }

  ~ThreadPool() {
    {
      unique_lock<mutex> lock(queue_mutex);
      stop.store(true);
    }
    condition.notify_all();
    for (thread &worker : workers) {
      worker.join();
    }
  }

  template <class F, class... Args>
  auto enqueue(F &&f, Args &&...args)
      -> future<typename result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();
    {
      std::unique_lock<std::mutex> lock(queue_mutex);

      if (stop)
        throw std::runtime_error("enqueue on stopped ThreadPool");

      tasks.emplace([task]() { (*task)(); });
    }
    condition.notify_one();
    return res;
  }
};

int main() {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    perror("socket failed");
    return 1;
  }

  // 设置地址重用
  int opt = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt))) {
    perror("setsockopt");
    return 1;
  }

  struct sockaddr_in address;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_family = AF_INET;
  address.sin_port = htons(PORT);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    return 1;
  }

  if (listen(server_fd, MAX_CONNECTIONS) < 0) {
    perror("listen");
    return 1;
  }

  int epoll_fd = epoll_create1(0);
  if (epoll_fd == -1) {
    perror("epoll_create1");
    return 1;
  }

  // 创建epoll事件, 监听server_fd
  struct epoll_event event;
  event.events = EPOLLIN;
  event.data.fd = server_fd;

  // 添加server_fd到epoll
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
    perror("epoll_ctl: server_fd");
    return 1;
  }

  struct epoll_event events[MAX_CONNECTIONS];

  SessionManager session_manager;
  ThreadPool thread_pool(4);
  // 事件循环
  while (true) {
    int nfds = epoll_wait(epoll_fd, events, MAX_CONNECTIONS, -1);
    if (nfds < 0) {
      perror("epoll_wait");
      return 1;
    } else if (nfds == 0) { // 超时
      continue;
    }

    for (int i = 0; i < nfds; i++) {
      int activate_fd = events[i].data.fd;
      if (events->events & EPOLLERR) {
        perror("epoll error");
        return 1;
      }
      // 处理新连接
      if (activate_fd == server_fd) {
        struct sockaddr_in client_address;
        socklen_t addrlen = sizeof(client_address);

        int client_fd =
            accept(server_fd, (struct sockaddr *)&client_address, &addrlen);
        if (client_fd == -1) {
          perror("accept");
          return 1;
        }

        cout << "New connection from " << client_fd << endl;

        // 添加新连接到epoll
        struct epoll_event event;
        event.events = EPOLLIN;
        event.data.fd = client_fd;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
          perror("epoll_ctl: client_fd");
          return 1;
        }
      } else {
        // 处理客户端请求
        char buffer[1024];
        int bytes_read = read(activate_fd, buffer, sizeof(buffer));

        // 读取失败
        if (bytes_read == -1) {
          perror("read");
          return 1;
        }
        // 客户端关闭连接
        else if (bytes_read == 0) {
          cout << "Client " << activate_fd << " disconnected" << endl;
          // 客户端关闭连接
          close(activate_fd);
          // 从epoll中删除
          epoll_ctl(epoll_fd, EPOLL_CTL_DEL, activate_fd, NULL);
          // 从等待队列中删除
          for (auto it = waiting_list.begin(); it != waiting_list.end(); it++) {
            if (it->second == activate_fd) {
              waiting_list.erase(it);
              break;
            }
          }
          // 从客户端和游戏会话ID的映射表中删除
          client_game_table.erase(activate_fd);
          continue;
        }

        // 处理客户端请求
        string data(buffer, bytes_read);
        GameAction action = GameAction::deserialize(data);
        if (action.type == ActionType::JOIN) {
          string match_code = string(action.data);

          // 查看等待队列中是否有匹配的玩家
          if (waiting_list.count(match_code) > 0) {

            // 匹配成功
            int client1 = waiting_list[match_code];
            int client2 = activate_fd;

            cout << "Match found between " << client1 << " and " << client2
                 << endl;

            // 从等待队列中删除
            waiting_list.erase(match_code);
            // 创建游戏会话
            unsigned long session_id =
                session_manager.create_session(client1, client2);
            // 更新客户端和游戏会话ID的映射表
            client_game_table[client1] = session_id;
            client_game_table[client2] = session_id;
            // 发送游戏开始信息
            string start_message = "Match Success";
            send(client1, start_message.c_str(), start_message.size(), 0);
            send(client2, start_message.c_str(), start_message.size(), 0);
            cout << "Game session ID: " << session_id << endl;
            cout << "Successfully notified clients\n";
          } else {
            cout << "No match found for client " << activate_fd << endl;
            // 匹配失败, 将玩家加入等待队列
            waiting_list[match_code] = activate_fd;
            // 发送等待信息
            string waiting_message = "Waiting for match...";
            send(activate_fd, waiting_message.c_str(), waiting_message.size(),
                 0);
          }
        } else {
          // 获取游戏会话ID
          unsigned long session_id = client_game_table[activate_fd];
          // 获取游戏
          ServerGame &game = session_manager.get_game(session_id);
          // 处理游戏请求
          thread_pool.enqueue([&game, action, activate_fd] {
            game.handlePlayerAction(action, activate_fd);
          });
        }
      }
    }
  }

  return 0;
}