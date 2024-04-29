#include "common/utilities.h"
#include <iostream>

using namespace std;

struct termios orig_termios;

// New function to display both boards side by side
void displayBoardsSideBySide(const Board &playerBoard, Board &opponentBoard,
                             bool showPlayerShips, int x, int y,
                             bool showOpponentShips) {
  clearScreen();
  cout << "Game going on!!" << endl;
  cout << endl;
  cout << "Now, you and your opponent will take turns attacking each other..."
       << endl;
  cout << "Both of your ships are hidden from the other " << RED << "until"
       << RESET_COLOR << " they are hit, shown as " << HIT_CELL << endl;
  cout << "Missed attempts will be labeled " << MISS_CELL << endl << endl;
  cout << "\"#\" shows where your next bomb targets at." << endl;
  cout << "Use " << ITALIC << UNDERLINE << "wasd" << RESET_COLOR << " or "
       << ITALIC << UNDERLINE << "arrow keys" << RESET_COLOR
       << " to adjust its location." << endl;
  cout << BOLD << GREEN << "#" << RESET_COLOR << " is a valid one, " << endl;
  cout << RED_BG << "#" << RESET_COLOR << " would be invalid." << endl;
  cout << "Press the " << BOLD << CYAN << "Enter" << RESET_COLOR << " key to "
       << RED << BOLD << "BOMBARD!!!" << RESET_COLOR << endl
       << endl;
  cout << "When you press " << BOLD << MAGENTA << "Esc" << RESET_COLOR << " or "
       << BOLD << MAGENTA << "q" << RESET_COLOR << ", \n"
       << "Your game status will be " << GREEN << "saved" << RESET_COLOR
       << ", and you will return to the main menu." << endl
       << "Have fun!" << endl
       << endl
       << endl;
  cout << "  0 1 2 3 4 5 6 7 8 9      0 1 2 3 4 5 6 7 8 9\n";
  for (int i = 0; i < BOARD_SIZE; ++i) {
    // Display player's board row
    cout << char('A' + i) << ' ';
    playerBoard.displayRow(i, showPlayerShips, -1);
    cout << "   "; // Space between boards

    // Display computer's board row
    cout << char('A' + i) << ' ';
    if (x == i) {
      opponentBoard.displayRow(i, false, y);
    } else {
      opponentBoard.displayRow(i, false, -1);
    }
    cout << endl;
  }
  cout << "    ships survived: " << GREEN << playerBoard.getShipsLeft()
       << RESET_COLOR << "      "
       << "ships survived: " << GREEN << opponentBoard.getShipsLeft()
       << RESET_COLOR << endl;
  cout << "       Your Board           Opponent's Board" << endl;
}

void clearScreen() { cout << "\033[2J\033[1;1H"; }

/**
 * @brief 清除屏幕上方的行数
 */
void clearLinesAbove(int numLines) {
  for (int i = 0; i < numLines; ++i) {
    // Move the cursor up
    std::cout << "\033[A";
    // Erase the line
    std::cout << "\033[K";
  }
}

// 获取终端的宽度
int getTerminalWidth() {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  return w.ws_col;
}

/**
 * @brief 禁用标准输入的缓冲区
 */
void enableRawMode() {
  tcgetattr(STDIN_FILENO, &orig_termios);
  struct termios raw = orig_termios;
  raw.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

/**
 * @brief 恢复标准输入的缓冲区
 */

void disableRawMode() { tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); }

// 获取居中打印字符串光标的位置
int getCenteredPosition(const string &text, int terminalWidth) {
  return (terminalWidth - text.length()) / 2;
}

// 居中打印字符串
void printCentered(const string &text, int terminalWidth) {
  int pos_x = getCenteredPosition(text, terminalWidth);
  cout << "\033[" << pos_x << "C";
  cout << text << endl;
}