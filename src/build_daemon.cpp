#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <iostream>
#include <fstream>
#include <boost/thread.hpp>
#include <boost/regex.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include "build_daemon.hpp"
#include "filters.hpp"
#include "ignore_list_reader.hpp"
#include "ascii.hpp"
#include "command_line.hpp"
#include "build_tool_selector.hpp"

class filesystem_tool_tester : public tool_tester {
  const boost::filesystem::path& base;
public:
  filesystem_tool_tester(const boost::filesystem::path& base) : base(base) { }

  virtual bool test(const char* filename) {
    return boost::filesystem::exists(base / filename);
  }
};


build_daemon::build_daemon(const char *path, const char* user_cmd) : watched_path(path), user_cmd(user_cmd), building(false), builder("make") {

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
    if (is::verbose_option(argv[arg])) {
      std::cout << "lazybuild version 0.4" << std::endl;
      return 0;
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

  vector<lazy::filesystem::event_filter> filters;
  /*
  filters.insert(filters.end(), filters.standard_filters);
  filters.insert(filters.end(), filters.load(boost::filesystem::current_path() / ".lazyignore"));
  filters.insert(filters.end(), filters.load(boost::filesystem::current_path() / ".gitignore"));
  
  if (boost::filesystem::exists(gitignore_path) && boost::filesystem::is_regular_file(gitignore_path)) {
    ignore_list_reader reader;
    
    ifstream f;
    
    f.open(gitignore_path.string());
    auto patterns = reader.read(f);
    f.close();
    for (auto &x : patterns) {
      cout << "Pattern " << x << endl;
    }
  }

*/
  build_daemon daemon(path, cmd);
  
  return daemon.run();
}


int build_daemon::run() {
  if (boost::filesystem::exists(watched_path)) {

    boost::filesystem::path canonical_path = boost::filesystem::canonical(watched_path);
    
    if (boost::filesystem::is_regular_file(canonical_path)) {
      canonical_path = canonical_path.parent_path();
    }
    
    if (!boost::filesystem::is_directory(canonical_path)) {
      std::cout << ASCII::red("error") << " path " << ASCII::red(canonical_path.string()) << " must be a directory" << std::endl;
      return -1;
    }
    
    std::cout << "Watching " << ASCII::green(canonical_path.string()) << std::endl;

    filesystem_tool_tester tester(canonical_path);

    tool_selector selector;

    builder = project_builder(user_cmd == NULL ? selector.select(tester) : user_cmd);
    std::cout << "Builder  " << ASCII::green(builder.build_command()) << std::endl;

    start_watching(canonical_path);
    return 0;
  } else {
    std::cerr << ASCII::red("error") << ": path '" << watched_path << "' does not exist" << std::endl;
    return -1;
  }
}

void event_cb(ConstFSEventStreamRef streamRef,
		void *ctx,
		size_t count,
		void *paths,
		const FSEventStreamEventFlags flags[],
		const FSEventStreamEventId ids[]) {
  build_daemon* daemon = (build_daemon*)ctx;
  
  daemon->callback(streamRef, count, paths, flags, ids);
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
    
    std::cout << "Build triggered by: " << ASCII::red(triggering_path) << std::endl;
    boost::thread t([&] {
	builder.build();
	std::cout << "Quiet period (1s) ..... ignoring events" << std::flush;
	boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
	std::cout << "\x1B[0G\x1B[0KWaiting for more changes..." << std::endl;
	building = false;
      });
    
    t.detach();

    return 0;
  }
}
