#pragma once

#include "server/server_game.h"
#include <ctime>
#include <random>
#include <unordered_map>

using namespace std;

typedef unsigned long SessionId;

class SessionManager {
public:
  // 会话和游戏的映射
  unordered_map<SessionId, ServerGame> games;
  // 构造函数
  SessionManager() = default;
  // 创建会话
  SessionId create_session(int client1, int client2);
  // 获取游戏
  ServerGame &get_game(SessionId session_id);

private:
  SessionId generate_session_id();
};