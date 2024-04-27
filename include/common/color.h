#ifndef COLOR_H
#define COLOR_H
#include <string>

const std::string BLACK = "\033[0;30m";
const std::string RED = "\033[0;31m";
const std::string GREEN = "\033[0;32m";
const std::string YELLOW = "\033[0;33m";
const std::string BLUE = "\033[0;34m";
const std::string GREY = "\033[0;37m";
const std::string MAGENTA = "\033[0;35m";
const std::string CYAN = "\033[0;36m";
const std::string RED_BG = "\033[1;41m";
const std::string BOLD = "\033[1m";
const std::string ITALIC = "\033[1;3m";
const std::string UNDERLINE = "\033[1;4m";
const std::string BLINKING = "\033[1;5m";
const std::string RESET_COLOR = "\033[0m";
#endif // COLOR_H