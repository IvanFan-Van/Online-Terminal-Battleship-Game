#ifndef GAME_ACTION_H
#define GAME_ACTION_H
#include "common/action.h"
#include <iostream>
using namespace std;

class GameAction {
public:
  ActionType type;
  string data;

  GameAction(ActionType t, const string &d);

  GameAction(ActionType t);

  GameAction();

  string serialize() const;

  static GameAction deserialize(const string &data);
};
#endif // GAME_ACTION_H