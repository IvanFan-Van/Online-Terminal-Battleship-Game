#include "common/action.h"

using namespace std;

std::ostream &operator<<(std::ostream &os, const ActionType &actionType) {
  switch (actionType) {
  case ActionType::SHOOT:
    os << "SHOOT";
    break;
  case ActionType::INIT:
    os << "INIT";
    break;
  case ActionType::CHECK_WIN:
    os << "CHECK_WIN";
    break;
  case ActionType::START:
    os << "START";
    break;
  case ActionType::GET_GAME_STATUS:
    os << "GET_GAME_STATUS";
    break;
  case ActionType::DEFAULT:
    os << "DEFAULT";
    break;
  case ActionType::JOIN:
    os << "JOIN";
    break;
  }
  return os;
}
