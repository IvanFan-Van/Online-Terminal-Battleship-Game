#include "common/game_action.h"

GameAction::GameAction(ActionType t, const string &d) : type(t), data(d) {}

GameAction::GameAction(ActionType t) : type(t) {}

GameAction::GameAction() {}

string GameAction::serialize() const {
  ostringstream oss;
  oss << to_string(static_cast<int>(type)) << "\n" << data;
  return oss.str();
}

GameAction GameAction::deserialize(const string &data) {
  istringstream iss(data);
  GameAction action;
  string type_str;
  iss >> type_str;
  try {
    action.type = static_cast<ActionType>(stoi(type_str));
  } catch (invalid_argument &e) {
    cout << "Invalid action type: " << type_str << endl;
  }

  iss.get(); // skip '\n'
  getline(iss, action.data, '\0');
  return action;
}

ostream &operator<<(ostream &os, const GameAction &action) {
  os << "Type: " << action.type << ", Data size: " << action.data.size();
  return os;
}