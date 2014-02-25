#include <string>
#include <unistd.h>
#include <iostream>

class project_builder {
  std::string command;
public:

  project_builder(std::string command) : command(command) {
  }

  int build() {
    return system(command.c_str());
  }
};
