#include "battleship/game.h"
#include "client/client_game.h"
#include "common/color.h"
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#define KEY_UP 65
#define KEY_DOWN 66
#define ENTER_KEY 10

using namespace std;

const vector<string> options = {
    "Computer", "Online", "Continue", "Tutorial", "Exit",
};
const int NUM_OPTIONS = options.size();

/**
 * @brief 打印进度条
 */

void printProgressBar(size_t current, size_t total) {
  // Progress bar width
  int barWidth = 70;

  // Calculate progress percentage
  float progress = static_cast<float>(current) / total;

  // Get terminal width
  int terminalWidth = getTerminalWidth();

  // Calculate padding
  int padding = (terminalWidth - barWidth) / 2;

  cout << "\r\033[K"; // Clear current line

  // Print padding
  std::cout << std::string(padding, ' ');

  // Print progress bar
  std::cout << "[";
  int pos = static_cast<int>(barWidth * progress);
  for (int i = 0; i < barWidth; ++i) {
    if (i < pos)
      std::cout << "=";
    else if (i == pos)
      std::cout << ">";
    else
      std::cout << " ";
  }
  std::cout << "] " << int(progress * 100.0) << " %" << std::flush;
}

/**
 * @brief 显示游戏的Logo
 */
void displayLogo(bool showAnimation) {
  const string art = R"(
        ____  ___  ______________    ___________ __  __________ 
       / __ )/   |/_  __/_  __/ /   / ____/ ___// / / /  _/ __ \
      / __  / /| | / /   / / / /   / __/  \__ \/ /_/ // // /_/ /
     / /_/ / ___ |/ /   / / / /___/ /___ ___/ / __  // // ____/ 
    /_____/_/  |_/_/   /_/ /_____/_____//____/_/ /_/___/_/      
                                                              
      )";

  // Split the art into lines
  std::istringstream artStream(art);
  std::vector<std::string> lines;
  std::string line;
  // 将LOGO分割成行
  while (std::getline(artStream, line)) {
    lines.push_back(line);
  }

  // Find the maximum line length to use for padding
  size_t max_length = 0;
  for (const auto &line : lines) {
    if (line.length() > max_length) {
      max_length = line.length();
    }
  }

  // 逐行打印LOGO
  int terminalWidth = getTerminalWidth();
  for (size_t col = 0; col <= max_length; ++col) {
    if (showAnimation) {
      for (const auto &line : lines) {
        printCentered(line.substr(0, col), terminalWidth);
      }

      // Print the progress bar at the bottom.
      printProgressBar(col, max_length);

      // Delay for demonstration purposes.
      std::this_thread::sleep_for(std::chrono::milliseconds(25));
    } else {
      for (const auto &line : lines) {
        printCentered(line, terminalWidth);
      }

      // Print the progress bar at the bottom.
      printProgressBar(1, 1);
    }

    // Move the cursor back to the top.
    std::cout << "\033[" << lines.size() << "A";
  }
  // After the final iteration, move the cursor below the art and progress bar
  std::cout << "\033[" << lines.size() + 1 << "B" << std::endl;
  if (showAnimation) {
    this_thread::sleep_for(
        chrono::milliseconds(500)); // Delay for demonstration purposes
  }
}

/**
 * @brief 显示游戏菜单
 */
void displayMenu(int selectedOption) {
  int terminalWidth = getTerminalWidth();

  std::string line1 = "Select Option:";
  std::string line4 =
      "(Using the arrow keys to navigate, press Enter to select)\n";

  for (size_t i = 0; i < options.size(); ++i) {
    string optionLine =
        ((int)i == selectedOption
             ? YELLOW + BOLD + "=> " + options[i] + " <=" + RESET_COLOR
             : "   " + options[i] + "   ");
    int padding = (terminalWidth - strlen("=> Computer <=")) / 2;
    cout << string(padding, ' ') << optionLine << endl;
  }

  // Calculate padding and print the instruction line
  int padding4 = (terminalWidth - line4.length()) / 2;
  cout << string(padding4, ' ') << line4 << endl;

  cout.flush();
}

/**
 * @brief 选择游戏模式
 */
void chooseMode(int &selectedOption) {

  while (true) {
    char c;
    if (read(STDIN_FILENO, &c, 1) == -1) {
      perror("read");
      exit(1);
    }

    if (c == '\033') { // Arrow keys are preceded by an escape sequence \033[
      read(STDIN_FILENO, &c, 1); // Skip the [
      read(STDIN_FILENO, &c, 1);
      if (c == KEY_UP) {
        if (selectedOption > 0) {
          selectedOption--;
        } else {
          selectedOption = NUM_OPTIONS - 1;
        }
      } else if (c == KEY_DOWN) {
        if (selectedOption < NUM_OPTIONS - 1) {
          selectedOption++;
        } else {
          selectedOption = 0;
        }
      }
    } else if (c == 'w') {
      if (selectedOption > 0) {
        selectedOption--;
      } else {
        selectedOption = NUM_OPTIONS - 1;
      }
    } else if (c == 's') {
      if (selectedOption < NUM_OPTIONS - 1) {
        selectedOption++;
      } else {
        selectedOption = 0;
      }
    } else if (c == ENTER_KEY) {
      break;
    }
    clearLinesAbove(NUM_OPTIONS + 2);
    displayMenu(selectedOption);
  }
}

int start(bool showAnimation) {
  // Disable standard input buffering
  enableRawMode();
  // clear screen
  clearScreen();
  // Display the logo
  displayLogo(showAnimation);
  // Default to computer mode
  int selectedOption = 0;
  // Display the menu
  displayMenu(selectedOption);
  // choose game mode
  chooseMode(selectedOption);
  // Restore terminal settings
  disableRawMode();

  return selectedOption;
}

// Option 1
void startOfflineGame() {
  printCentered("Starting Game With Computer...", getTerminalWidth());
  sleep(1); // Delay for demonstration purposes
  Game offlineGame = Game();
  offlineGame.start();
  cout << "Exit game\n";
}
// Option 2
void startOnlineGame() {
  printCentered("Starting Game With People...", getTerminalWidth());
  sleep(1); // Delay for demonstration purposes
  // string DEFAULT_IP = "43.143.114.119";
  string DEFAULT_IP = "127.0.0.1";
  // 匹配成功
  ClientGame onlineGame = ClientGame(DEFAULT_IP);
  onlineGame.start();

  cout << "Closing Game...\n";
  onlineGame.stop();
}
// Option 3
void continueOfflineGame() {
  Game offlineGame = Game();
  string filePath;
  cout << "Load game from(default "
          "is 'savegame.txt'): ";
  getline(cin, filePath);
  if (filePath.empty()) {
    filePath = "savegame.txt";
  }
  bool success = offlineGame.loadGame(filePath);
  if (!success) {
    cout << "Enter any key to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
  } else {
    offlineGame.start();
  }
}
// Option 4
void displayTutorial() {
  clearScreen();
  const string title = R"(
            
        _______ _    _ _______ ____  _____  _____          _      
       |__   __| |  | |__   __/ __ \|  __ \|_   _|   /\   | |     
          | |  | |  | |  | | | |  | | |__) | | |    /  \  | |     
          | |  | |  | |  | | | |  | |  _  /  | |   / /\ \ | |     
          | |  | |__| |  | | | |__| | | \ \ _| |_ / ____ \| |____ 
          |_|   \____/   |_|  \____/|_|  \_\_____/_/    \_\______|
                                                                  
                                                                  
      )";
  printCentered(title, getTerminalWidth());

  const string text = R"(
        Game Rules:
        1.  Each player has a fleet of ships placed on a 10x10 grid.
        2.  The ship consists of 
            - one aircraft carrier (4 units), 
            - two battleships (3 units), 
            - three cruisers (2 units), 
            - four submarines (1 unit).
        3.  Players take turns guessing the coordinates to target their opponent's ships.
        4.  The grid is marked with hits ('X') and misses ('O') to record the shots.
        5.  When all cells of a ship are hit, it is considered sunk.
        6.  The first player to sink all of their opponent's ships wins the game.

        Controls:
        1.  First, place your own ships.
              Use "WASD" or arrow keys to navigate the board, 
              press the [  spacebar  ] to rotate the ship (with the upperleft corner fixed),
              and press ENTER to select.

        2.  Then, start attacking. You'll see your board on the left with the opponent's on the right.
              Use "WASD" or arrow keys to navigate the board,
              and press ENTER to select.
      )";
  printCentered(text, getTerminalWidth());
  cout << "\n\n";
  cout << "Enter any key to continue...";
  cin.ignore(numeric_limits<streamsize>::max(), '\n');
}
// Option 5
void exit() {
  printCentered("Thank You for Playing Battleship!", getTerminalWidth());
}

int main(int argc, char *argv[]) {
  bool showAnimation = true;
  while (true) {
    // Start the game
    int option = start(showAnimation);
    // cout << "option: " << option << endl;
    switch (option) {
    case 0: {
      startOfflineGame();
      showAnimation = true;
      break;
    }
    case 1: {
      startOnlineGame();
      showAnimation = true;
      break;
    }
    case 2: {
      continueOfflineGame();
      showAnimation = false;
      break;
    }
    case 3: {
      displayTutorial();
      showAnimation = false;
      break;
    }
    case 4: {
      exit();
      return 0;
    }
    default: {
      cout << "Invalid Option\n";
    }
    }
  }

  return 0;
}