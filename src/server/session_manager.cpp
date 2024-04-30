#include "server/session_manager.h"

SessionId SessionManager::create_session(int client1, int client2) {
  SessionId session_id = generate_session_id();
  ServerGame game(client1, client2);
  games[session_id] = game;
  return session_id;
}

ServerGame &SessionManager::get_game(SessionId session_id) {
  return games[session_id];
}

SessionId SessionManager::generate_session_id() {
  // 随机数生成器
  static random_device rd;
  // 生成器
  static mt19937_64 generator(rd());
  uniform_int_distribution<SessionId> distribution;

  SessionId session_id = 0;
  do {
    session_id = distribution(generator);
  } while (games.count(session_id) > 0);

  return session_id;
}