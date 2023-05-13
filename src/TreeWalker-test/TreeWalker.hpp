#pragma once

#include <FS.h>
#include <Functor.h>

// Visitor shall return false to stop the walk
typedef Functor2wRet<fs::FS&, fs::File &, bool> Visitor;

class TreeWalker {
public:
  TreeWalker(const Visitor &visitor) : visitor_(visitor){};
  uint32_t begin(FS &fs, const char *dirName = "/", uint8_t levels = 5) {
    visited_ = 0;
    fs_ = &fs;
    File current_dir = fs.open(dirName);
    if (!current_dir) {
      log_e("failed to open directory '%s'", dirName);
      return false;
    }
    if (!current_dir.isDirectory()) {
      log_e("not a directory: '%s'", dirName);
      return false;
    }
    walkDirByFile_(current_dir, levels - 1);
    return visited_;
  }

private:
  Visitor visitor_;
  uint32_t visited_;
  fs::FS *fs_;

  void walkDirByFile_(File dir, uint8_t levels) {
    File fd;
    while ((fd = dir.openNextFile())) {
      visited_++;
      if (visitor_) {
        if (!visitor_(*fs_,fd)) {
            return;
        }
      }
      if (levels) {
        walkDirByFile_(fd, levels - 1);
      }
    }
  }
};
