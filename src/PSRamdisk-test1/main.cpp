
#ifdef M5UNIFIED
#include <M5Unified.h>
#endif

#include <PSRamFS.h>
//#include "./pfs.h"

void esp32Info(void);

void setup() {
  Serial.begin(115200);
  esp32Info();
  if (!PSRamFS.begin()) {
    log_e("PSRamFS Mount Failed");
    return;
  }
  if (PSRamFS.exists("/test.txt")) {
    PSRamFS.remove("/test.txt");;
  }
  File t = PSRamFS.open("/test.txt", FILE_WRITE);
  if (t) {
    t.print("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    Serial.print("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    Serial.printf("\n/test.txt size= %d \n", t.size());
    t.close();
  } else {
    Serial.println("Error: file could not be opened, halting");
    while(1) vTaskDelay(1);
  }
  File r = PSRamFS.open("/test.txt", FILE_READ);
  int rsize = r.size();
  if (r) {
    int c = 0;
    char rbuf[32];
    int len = 0;
    int total = 0;
    do {
      len = r.readBytes(rbuf, sizeof(rbuf));
      for (int i = 0; i < len; i++) {
        Serial.print(rbuf[i]);
      }
      Serial.printf("\nlen=%d", len);
      total += len;
    } while (len);
    t.close();
    Serial.printf("\ntotal=%d", total);
    if (total != rsize) {
      Serial.printf("\nwe can read all data except file end..Z");
    }
  }
  Serial.println("\nEND");
}

void loop() {
  // put your main code here, to run repeatedly:

}
void esp32Info(void) {
  Serial.println("---------------------------- -");
  uint64_t chipid;
  chipid = ESP.getEfuseMac(); //The chip ID is essentially its MAC address(length: 6 bytes).
  Serial.printf("ESP32 Chip ID = % 04X\r\n", (uint16_t)(chipid >> 32)); //print High 2 bytes
  Serial.printf("Chip Revision % d\r\n", ESP.getChipRevision());
  esp_chip_info_t chip_info;
  esp_chip_info(&chip_info);
  Serial.printf("Number of Core: % d\r\n", chip_info.cores);
  Serial.printf("CPU Frequency: % d MHz\r\n", ESP.getCpuFreqMHz());
  Serial.printf("Flash Chip Size = % d byte\r\n", ESP.getFlashChipSize());
  Serial.printf("Flash Frequency = % d Hz\r\n", ESP.getFlashChipSpeed());
  Serial.printf("Free Heap Size = % d\r\n", esp_get_free_heap_size());
  Serial.printf("Total PSRAM: %d\r\n", ESP.getPsramSize());
  Serial.printf("Free PSRAM: %d\r\n", ESP.getFreePsram());
  Serial.printf("ESP - IDF version = % s\r\n", esp_get_idf_version());
  Serial.println();
}