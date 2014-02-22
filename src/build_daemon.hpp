#pragma once

#include "project_builder.h"

class build_daemon {
  project_builder &builder;
  const char *watched_path;
  bool building;

  build_daemon(const char *path, project_builder &builder);

  std::string make_absolute_path(const char *initial_path);

public:
  static int run(int argc, char *argv[]);

  int run();

  int build();
};
