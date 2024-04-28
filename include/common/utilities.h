#ifndef BATTLESHIP_UTILITIES_H
#define BATTLESHIP_UTILITIES_H

#include "battleship/board.h"
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

extern struct termios orig_termios;

void displayBoardsSideBySide(const Board &playerBoard, Board &opponentBoard,
                             bool showPlayerShips, int x, int y,
                             bool showOpponentShips = false);

void clearScreen();

/**
 * @brief 清除屏幕上方的行数
 */
void clearLinesAbove(int numLines);

// 获取终端的宽度
int getTerminalWidth();

/**
 * @brief 禁用标准输入的缓冲区
 */
void enableRawMode();

/**
 * @brief 恢复标准输入的缓冲区
 */

void disableRawMode();

int getCenteredPosition(const string &text, int terminalWidth);

void printCentered(const string &text, int terminalWidth);

#endif // BATTLESHIP_UTILITIES_H