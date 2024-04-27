#ifndef BATTLESHIP_UTILITIES_H
#define BATTLESHIP_UTILITIES_H

#include "battleship/board.h"

void displayBoardsSideBySide(const Board &playerBoard, Board &opponentBoard,
                             bool showPlayerShips, int x, int y,
                             bool showOpponentShips = false);

void clearScreen();

void clearBoard();

int getCenteredPosition(const string &text, int terminalWidth);

void printCentered(const string &text, int terminalWidth);

#endif // BATTLESHIP_UTILITIES_H