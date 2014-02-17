#include <libgen.h>
#include <stdio.h>
#include <string.h>

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <iostream>


#include "build_daemon.hpp"

void event_cb(ConstFSEventStreamRef streamRef,
              void *ctx,
              size_t count,
              void *paths,
              const FSEventStreamEventFlags flags[],
              const FSEventStreamEventId ids[]) {

  std::cout << "Event raised " << std::endl;

  file_paths_t *file_paths = (file_paths_t *) ctx;
  size_t i;
  size_t ignored_paths = 0;

  for (i = 0; i < count; i++) {
    char *path = ((char **) paths)[i];
    /* flags are unsigned long, IDs are uint64_t */
    printf("Change %llu in %s, flags %lu\n", ids[i], path, (long) flags[i]);
    size_t j;
    for (j = 0; j < file_paths->len; j++) {
      char *file_path = file_paths->paths[j];
      printf("%s %s\n", file_path, path);
      if (strcmp(file_path, path) == 0) {
        printf("File %s changed.\n", file_path);
        exit(0);
      }
    }

    /* TODO: this logic is wrong */
    ignored_paths++;
  }
  if (count > ignored_paths) {
    /* OS X occasionally leaks event streams. Manually stop the stream just to make sure. */
    FSEventStreamStop((FSEventStreamRef) streamRef);
    exit(0);
  }
}


std::string build_daemon::make_absolute_path(char *initial_path) {
  char *path = realpath(initial_path, NULL);
  char *dir_path = NULL;
  if (path == NULL) {
    path = dirname(initial_path);

    if (strcmp(path, ".") == 0) {
      /* realpath(".") returns a useless path like /User */
      dir_path = realpath("./", NULL);
    } else {
      dir_path = realpath(path, NULL);
    }

    if (dir_path == NULL) {
      fprintf(stderr, "Error %i in realpath(\"%s\"): %s\n", errno, path, strerror(errno));
      exit(1);
    }
    char *file_name = basename(initial_path);
    asprintf(&path, "%s/%s", dir_path, file_name);
  }

  return path;
}

int build_daemon::run(int argc, char *argv[]) {
  std::cout << argv[0] << std::endl;
  if (argc < 2) {
    printf("No path specified.\n");
    printf("Usage: %s path/to/watch command-to-run\n", argv[0]);
    return 1;
  }

  project_builder builder("make");

  build_daemon daemon(argv[1], builder);

  return daemon.run();

}

build_daemon::build_daemon(char *path, project_builder builder) : watched_path(path), builder(builder) {
}

int build_daemon::run() {
  std::string path = make_absolute_path(watched_path);

  file_paths_t *file_paths = (file_paths_t *) malloc(sizeof(file_paths_t));
  file_paths->len = 0;
  file_paths->size = 2;
  file_paths->paths = (char **) malloc(file_paths->size * sizeof(char *));


  std::cout << "Watching " << path << std::endl;

  CFMutableArrayRef paths = CFArrayCreateMutable(NULL, 2, NULL);
  auto cfs_path = CFStringCreateWithCString(NULL, path.c_str(), kCFStringEncodingUTF8);
  CFArrayAppendValue(paths, cfs_path); /* pretty sure I'm leaking this */

  FSEventStreamContext ctx = {
      0,
      file_paths,
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
