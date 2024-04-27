#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "color.h"
#include <string>

const int BOARD_SIZE = 10;
// const int SHIPS[] = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1};
const int SHIPS[] = {4, 3, 2, 1};
// Cell types
const char EMPTY = '.';
const char SHIP = 'S';
const char HIT = 'X';
const char MISS = 'O';
// Colored cells
const std::string EMPTY_CELL = RESET_COLOR + EMPTY + RESET_COLOR;
const std::string SHIP_CELL = BLUE + SHIP + RESET_COLOR;
const std::string HIT_CELL = RED + HIT + RESET_COLOR;
const std::string MISS_CELL = GREY + MISS + RESET_COLOR;

const int STARTPOINT = 4;
const int MAX_SIZE = 1024;

#endif // CONSTANTS_H