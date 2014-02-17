#pragma once

#include "project_builder.h"

typedef struct {
  size_t len;
  size_t size;
  char **paths;
} file_paths_t;

class build_daemon {
  project_builder& builder;
  const char* watched_path;
  bool building;

  build_daemon(const char* path, project_builder& builder);

  std::string make_absolute_path(const char *initial_path);

public:
  static int run(int argc, char *argv[]);

  int run();
  int build();
};
