#pragma once

#include <boost/filesystem/path.hpp>

#include "project_builder.h"

class build_daemon {
  project_builder &builder;
  const char *watched_path;
  bool building;

  build_daemon(const char *path, project_builder &builder);

  void start_watching(boost::filesystem::path path);

public:
  static int run(int argc, char *argv[]);

  int run();

  int build(const char * triggering_path);
};
