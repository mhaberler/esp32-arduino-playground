#ifdef M5UNIFIED
#include <M5Unified.h>
#include <SD.h>
static constexpr const gpio_num_t SDCARD_CSPIN = GPIO_NUM_4;
#endif
#include <PSRamFS.h>

#include <Arduino.h>
#include <Fmt.h>
#include <map>

#include "TreeWalker.hpp"
#include <fmt/chrono.h>

#include <LittleFS.h>

Fmt out(Serial);

uint32_t mS, total_bytes, total_files;

bool ls = true; // false;

bool toPsram(fs::FS &sourceFS, const char *path);

struct PsRamFile_t {
  char *data;
  size_t size;
  RomDiskStream getStream() {
    return RomDiskStream((const uint8_t *)data, size);
  }
};

std::map<String, PsRamFile_t> MyPsramFiles; // global psram-files storage

bool toPsram(fs::FS &sourceFS, const char *path) {
  fs::File myFile = sourceFS.open(path);

  if (!myFile) {
    log_e("toPsram: failed to open '%s'", path);
    return false;
  }
  size_t fsize = myFile.size() + 1;
  if (ESP.getMaxAllocPsram() < fsize) {
    log_e("file larger than  getMaxAllocPsram): %u/%u\n", fsize + 1,
          ESP.getMaxAllocPsram());
    return false;
  }
  auto data_mem = ((char *)heap_caps_malloc(fsize, MALLOC_CAP_SPIRAM));
  if (data_mem == NULL) {
    log_e("failed to alloc %u\n", fsize);

    myFile.close();
    return false;
  }
  PsRamFile_t myPsRamFile = {.data = data_mem, .size = myFile.size()};
  size_t bytes_read = myFile.readBytes(myPsRamFile.data, myFile.size());
  bool result = (myFile.size() == bytes_read);
  if (!result) {
    // incomplete copy ?
    log_w("[File copy missed %d bytes for %s\n", myFile.size() - bytes_read,
          path);
    heap_caps_free(data_mem);
  } else {
    total_bytes += bytes_read;
    total_files++;
    MyPsramFiles[path] = myPsRamFile; // add file to global psram-files array
  }
  myFile.close();
  return result;
}

bool visit(fs::FS &fs, fs::File &f) {
  time_t t = f.getLastWrite();
  struct tm *tm = localtime(&t);
  bool isDir = f.isDirectory();
  if (ls)
  out.fmtln("{} {:>7} {:<50} {:%Y-%m-%d %H:%M:%S}", isDir ? "d" : "f", f.size(),
            f.path(), *tm);
  if (isDir) {
    return true;
  }
  bool result = toPsram(fs, f.path());
  if (!result) {
    out.fmtln("toPsram() failed, stopping");
  }
  return result;
}

void setup() {
#ifdef M5UNIFIED
  auto cfg = M5.config();
  M5.begin(cfg);
#else
  Serial.begin(115200);
#endif
  while (!Serial) {
    yield();
  }

  esp_log_level_set("*", ESP_LOG_WARN);

  uint32_t pre = ESP.getFreePsram();
  out.fmtln("pre: free PSRAM={}", pre);

  const Visitor v = makeFunctor((Visitor *)NULL, visit);
  TreeWalker walker(v);

  LittleFS.begin();

  uint32_t now = millis();

  walker.begin(LittleFS, "/", 10);

  mS = millis() - now;
  uint32_t post = ESP.getFreePsram();

  out.fmtln("post: free PSRAM={} used={}", post, pre - post);
  out.fmtln("{} files, {} bytes cached in {:.3f} S - {:.3f} kB/s", total_files,
            total_bytes, mS / 1000.0, (total_bytes / 1024.0) / (mS / 1000.0));

  // SD.begin(SDCARD_CSPIN, SPI, 25000000);
  // walker.begin(SD, "/", 10);
}

void loop() {}

extern "C" {
void fmtlib_error(const char *file, int line, const char *message) {
  log_e("%s:%d: assertion failed: %s\n", file, line, message);
}
}
