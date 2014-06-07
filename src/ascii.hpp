#include <string>

using namespace std;

class ASCII {
public:
  static string red(const char *text) {
    return string("\x1b[31;1m") + text + "\x1b[0m";
  }

  static string red(const string &text) {
    return string("\x1b[31;1m") + text + "\x1b[0m";
  }

  static string green(const char *text) {
    return string("\x1b[32;1m") + text + "\x1b[0m";
  }

  static string green(const string &text) {
    return string("\x1b[32;1m") + text + "\x1b[0m";
  }
};
