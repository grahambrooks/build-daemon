#pragma once

#include "project_builder.h"

typedef struct {
  size_t len;
  size_t size;
  char **paths;
} file_paths_t;


/*
   kFSEventStreamEventFlagNone = 0x00000000,
   kFSEventStreamEventFlagMustScanSubDirs = 0x00000001,
   kFSEventStreamEventFlagUserDropped = 0x00000002,
   kFSEventStreamEventFlagKernelDropped = 0x00000004,
   kFSEventStreamEventFlagEventIdsWrapped = 0x00000008,
   kFSEventStreamEventFlagHistoryDone = 0x00000010,
   kFSEventStreamEventFlagRootChanged = 0x00000020,
   kFSEventStreamEventFlagMount = 0x00000040,
   kFSEventStreamEventFlagUnmount = 0x00000080, // These flags are only set if you specified the FileEvents
 // flags when creating the stream.
   kFSEventStreamEventFlagItemCreated = 0x00000100,
   kFSEventStreamEventFlagItemRemoved = 0x00000200,
   kFSEventStreamEventFlagItemInodeMetaMod = 0x00000400,
   kFSEventStreamEventFlagItemRenamed = 0x00000800,
   kFSEventStreamEventFlagItemModified = 0x00001000,
   kFSEventStreamEventFlagItemFinderInfoMod = 0x00002000,
   kFSEventStreamEventFlagItemChangeOwner = 0x00004000,
   kFSEventStreamEventFlagItemXattrMod = 0x00008000,
   kFSEventStreamEventFlagItemIsFile = 0x00010000,
   kFSEventStreamEventFlagItemIsDir = 0x00020000,
   kFSEventStreamEventFlagItemIsSymlink = 0x00040000
*/

class build_daemon {
  project_builder builder;
  char* watched_path;


  build_daemon(char *string, project_builder builder);

  std::string make_absolute_path(char *initial_path);

public:
  static int run(int argc, char *argv[]);

  int run();
};
