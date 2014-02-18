#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <iostream>
#include <boost/thread.hpp>

#include "build_daemon.hpp"
#include <boost/regex.hpp>

void event_cb(ConstFSEventStreamRef streamRef,
              void *ctx,
              size_t count,
              void *paths,
              const FSEventStreamEventFlags flags[],
              const FSEventStreamEventId ids[]) {

  boost::regex e(".*/\\..*");
  
  auto interesting_mask = kFSEventStreamEventFlagItemCreated | kFSEventStreamEventFlagItemRemoved | kFSEventStreamEventFlagItemRenamed | kFSEventStreamEventFlagItemModified;
  
  
  for (int i = 0; i < count; i++) {
    char *path = ((char **) paths)[i];
    if ((flags[i] & interesting_mask) != 0) {
      
      if (boost::regex_match(path, e)) {
	// std::cout << "Ignorning change for " << path << std::endl;
      } else {
	// std::cout << "Acting on change for " << path << std::endl;
	build_daemon* daemon = (build_daemon*)ctx;
	daemon->build();
      }
    }
  }
}


std::string build_daemon::make_absolute_path(const char *initial_path) {
  char *path = (char*)malloc(PATH_MAX);

  if (realpath(initial_path, path) == NULL) {
    strcpy(path, initial_path);
    char *file_name = basename(path);
    path = dirname(path);

    char *dir_path = NULL;

    if (strcmp(path, ".") == 0) {
      dir_path = realpath("./", NULL); // realpath(".") returns a useless path like /User
    } else {
      dir_path = realpath(path, NULL);
    }

    if (dir_path == NULL) {
      fprintf(stderr, "Error %i in realpath(\"%s\"): %s\n", errno, path, strerror(errno));
      exit(1);
    }

    asprintf(&path, "%s/%s", dir_path, file_name);
    free(dir_path);
  }

  std::string s(path);
  free(path);

  return s;
}

int build_daemon::run(int argc, char *argv[]) {
  const char* path = argc > 1 ? argv[1] : "."; 
    
  project_builder builder(argc > 2 ? argv[2] : "make");

  build_daemon daemon(path, builder);

  return daemon.run();
}

build_daemon::build_daemon(const char *path, project_builder& builder) : watched_path(path), builder(builder), building(false) {
}

int build_daemon::run() {
  std::string path = make_absolute_path(watched_path);

  std::cout << "Watching " << path << std::endl;

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

  return 0;
}

int build_daemon::build() {
  if (building) {
    return 0;
  } else {
    building = true;
    boost::thread t([&] {
	builder.build();
	std::cout << "Sleeping" << std::endl;
	building = false;
      });
    
    t.detach();

    return 0;
  }
}
