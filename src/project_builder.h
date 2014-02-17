#include <string>
#include <unistd.h>
#include <iostream>

class project_builder {
  std::string command;
  bool building;
public:
  project_builder(std::string command) : command(command), building(false) {
  }

  void build() {
    if (!building) {
      pid_t pid = fork();

      if (pid == -1) {
        std::cout << "Unable to fork child process" << std::endl;
      }
      else if (pid == 0) {
        building = true;
        auto result = system(command.c_str());
        building = false;
      }
    }
  }
};
