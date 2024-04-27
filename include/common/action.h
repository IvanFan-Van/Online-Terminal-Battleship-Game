#ifndef ACTION_H
#define ACTION_H

#include "battleship/board.h"
#include "common/constants.h"
#include <cstring>
#include <sstream>
#include <string>

using namespace std;

enum ActionType { SHOOT, INIT, CHECK_WIN, START, GET_GAME_STATUS, DEFAULT };

#endif // ACTION_H