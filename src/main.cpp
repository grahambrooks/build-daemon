#include <iostream>
#include "build_daemon.hpp"


int main(int argc, char *argv[]) {
  build_daemon daemon;

  return daemon.run(argc, argv);
}
   
