#include "battleship/keyboard.h"
#include <termios.h>
#include <unistd.h>

char getKey();
void disableBuffering(termios &oldSettings);
void restoreSettings(const termios &oldSettings);

using namespace std;

char keyboard() {
  termios oldSettings;
  disableBuffering(oldSettings);

  char key = getKey();

  restoreSettings(oldSettings);
  return key;
}

void disableBuffering(termios &oldSettings) {
  termios newSettings;
  // STDIN_FILENO: Standard input file descriptor (0)
  tcgetattr(STDIN_FILENO, &oldSettings);
  newSettings = oldSettings;
  // ICANON: Enable canonical mode (line buffering)
  // ECHO: Enable echoing of input characters
  newSettings.c_lflag &= ~(ICANON | ECHO); // Disable line buffering and echoing
  newSettings.c_cc[VTIME] = 0;
  // Wait for at least one character
  newSettings.c_cc[VMIN] = 1;
  // TCSANOW: Terminal Control Set Attributes Now
  tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
}

void restoreSettings(const termios &oldSettings) {
  tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);
}

char getKey() {
  char key;
  while (true) {
    read(STDIN_FILENO, &key, 1);
    switch (key) {
    case ' ':
      return 'C';
    case '\n':
      return 'Y';
    case 'w':
    case 'W':
      return 'w';
    case 'a':
    case 'A':
      return 'a';
    case 's':
    case 'S':
      return 's';
    case 'd':
    case 'D':
      return 'd';
    case 'q':
    case 'Q':
      return 'N';
    case '\033':
      if (read(STDIN_FILENO, &key, 1) == -1)
        return 'N'; // ESC key pressed
      if (key == '[') {
        if (read(STDIN_FILENO, &key, 1) == -1)
          return 'N';
        switch (key) {
        case 'A':
          return 'w';
        case 'B':
          return 's';
        case 'C':
          return 'd';
        case 'D':
          return 'a';
        }
      }
      break;
    }
  }
}