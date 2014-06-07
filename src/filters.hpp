#pragma once

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <vector>
#include <boost/regex.hpp>

using namespace std;
namespace lazy {
  namespace filesystem {
    class event_filter {
    public:
      virtual bool should_ignore(const char *path, FSEventStreamEventFlags flags) {
        return false;
      }
    };

    class path_event_filter : public event_filter {
      boost::regex re;
    public:
      path_event_filter(boost::regex re) : re(re) {
      }

      virtual bool should_ignore(const char *path, FSEventStreamEventFlags flags) {
        return boost::regex_match(path, re);
      }

      static vector<event_filter> load(const string &path);
    };
  }
}


class filters {
  typedef std::function<bool(const char *path, FSEventStreamEventFlags flags)> filter;
public:
  filter hidden_file_filter = [](const char *path, FSEventStreamEventFlags flags) -> bool {
      boost::regex e(".*/\\..*");

      return boost::regex_match(path, e);
  };

  filter emacs_temp_file_filter = [](const char *path, FSEventStreamEventFlags flags) -> bool {
      boost::regex e(".*/#.*");

      return boost::regex_match(path, e);
  };

  filter file_action = [](const char *path, FSEventStreamEventFlags flags) {
      return ((kFSEventStreamEventFlagItemCreated |
          kFSEventStreamEventFlagItemRemoved |
          kFSEventStreamEventFlagItemRenamed |
          kFSEventStreamEventFlagItemModified) & flags) == 0;
  };

  filter no_symbolic_links = [](const char *path, FSEventStreamEventFlags flags) {
      return (kFSEventStreamEventFlagItemIsSymlink & flags) != 0;
  };

  bool should_ignore(const char *path, FSEventStreamEventFlags flags) {
    return hidden_file_filter(path, flags) ||
        no_symbolic_links(path, flags) ||
        file_action(path, flags) ||
        emacs_temp_file_filter(path, flags);
  }
};

