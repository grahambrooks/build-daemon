#pragma once

#include <boost/filesystem/path.hpp>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>

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

  void callback(ConstFSEventStreamRef streamRef,
		size_t count,
		void *paths,
		const FSEventStreamEventFlags flags[],
		const FSEventStreamEventId ids[]);

  int build(const char * triggering_path);
};
