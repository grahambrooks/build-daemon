#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <iostream>
#include <boost/thread.hpp>
#include <boost/regex.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include "build_daemon.hpp"
#include "filters.hpp"

void event_cb(ConstFSEventStreamRef streamRef,
              void *ctx,
              size_t count,
              void *paths,
              const FSEventStreamEventFlags flags[],
              const FSEventStreamEventId ids[]) {
  build_daemon* daemon = (build_daemon*)ctx;
  
  daemon->callback(streamRef, count, paths, flags, ids);
}

void build_daemon::callback(ConstFSEventStreamRef streamRef,
			    size_t count,
			    void *paths,
			    const FSEventStreamEventFlags flags[],
			    const FSEventStreamEventId ids[]) {
  filters filters;

  for (int i = 0; i < count; i++) {
    char *path = ((char **) paths)[i];
    if (filters.should_ignore(path, flags[i])) {
      // std::cout << "Ignorning change for " << path << std::endl;
    } else {
      // std::cout << "Acting on change for " << path << std::endl;
      build(path);
    }
  }
}

int build_daemon::run(int argc, char *argv[]) {
  const char * path = NULL;
  const char * cmd = NULL;

  for (auto arg = 1; arg < argc; arg++) {

    if (argv[arg][0] == '-') {
      if (argv[arg][1] == 'v') {
	std::cout << "lazybuild version 0.3" << std::endl;
	return 0;
      }
    } else {
      if (path == NULL) {
	path = argv[arg];
      } else {
	cmd = argv[arg];
      }
    }
  }
  
  if (path == NULL) {
    path = ".";
  }

  if (cmd == NULL) {
    cmd = "make";
  }

  project_builder builder(cmd);
  
  build_daemon daemon(path, builder);
  
  return daemon.run();
}

build_daemon::build_daemon(const char *path, project_builder& builder) : watched_path(path), builder(builder), building(false) {
}

int build_daemon::run() {
  boost::filesystem::path canonical_path = boost::filesystem::canonical(watched_path);

  if (boost::filesystem::is_regular_file(canonical_path)) {
    canonical_path = canonical_path.parent_path();
  }

  if (!boost::filesystem::is_directory(canonical_path)) {
    std::cout << "Path " << canonical_path << " does not exist or is not a directory" << std::endl;
    return -1;
  }

  std::cout << "Watching " << canonical_path << std::endl;

  start_watching(canonical_path);

  return 0;
}

void build_daemon::start_watching(boost::filesystem::path path) {
  CFMutableArrayRef paths = CFArrayCreateMutable(NULL, 2, NULL);
  auto cfs_path = CFStringCreateWithCString(NULL, path.c_str(), kCFStringEncodingUTF8);
  CFArrayAppendValue(paths, cfs_path);

  FSEventStreamContext ctx = {
      0,
      this,
      NULL,
      NULL,
      NULL
  };

  FSEventStreamCreateFlags flags = kFSEventStreamCreateFlagFileEvents;

  FSEventStreamRef stream = FSEventStreamCreate(NULL, &event_cb, &ctx, paths, kFSEventStreamEventIdSinceNow, 0, flags);
  FSEventStreamScheduleWithRunLoop(stream, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
  FSEventStreamStart(stream);

  CFRunLoopRun();
}

int build_daemon::build(const char * triggering_path) {
  if (building) {
    // std::cout << "Already running build: " << triggering_path << " change ignored" << std::endl;
    return 0;
  } else {
    building = true;
    
    std::cout << "Build triggered by: " << triggering_path << std::endl;
    boost::thread t([&] {
	builder.build();
	std::cout << "Quiet period ..... ignoring events" << std::flush;
	boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
	std::cout << "\x1B[0G\x1B[0KSleeping" << std::endl;
	building = false;
      });
    
    t.detach();

    return 0;
  }
}
