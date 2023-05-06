// see  lib/ESP32-OTA-Pull/examples/Basic-OTA-Example/Basic-OTA-Example.ino

#ifdef M5UNIFIED
#include <M5Unified.h>
#endif

#include <Arduino.h>
#include <Version.h>
// #define OTA_PULL_BOARD BOARD

const char *CurrentVersion = VERSION;
const char *jsonUrl = JSON_URL;
const char *board = OTA_PULL_BOARD;
// const char *device = DEVICE;

#include "ESP32OTAPull.h"

const char *errtext(int code);
void DisplayInfo();

void callback(int offset, int totallength);

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

#if defined(LED_BUILTIN)
  pinMode(LED_BUILTIN, OUTPUT);
#endif

  DisplayInfo();

  Serial.printf("Connecting to WiFi '%s'...", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (!WiFi.isConnected()) {
    Serial.print(".");
    delay(250);
  }
  Serial.printf("\n\n");

  // First example: update should NOT occur, because Version string in JSON
  // matches local VERSION value.
  ESP32OTAPull ota;

  // ota.OverrideBoard(board);
  // ota.OverrideDevice(device);
  // ota.AllowDowngrades(true);

  ota.SetCallback(callback);
  Serial.printf(
      "We are running version %s of the sketch, Board='%s', Device='%s'.\n",
      CurrentVersion, OTA_PULL_BOARD, WiFi.macAddress().c_str());
  Serial.printf("Checking %s to see if an update is available...\n", jsonUrl);

  // just check, do not actually update
  int ret =
      ota.CheckForOTAUpdate(jsonUrl, CurrentVersion); // , ota.DONT_DO_UPDATE);
  Serial.printf("CheckForOTAUpdate returned %d (%s)\n\n", ret, errtext(ret));

#ifdef FORCE_UPDATE
  delay(3000);

  // Second example: update *will* happen because we are pretending we have an
  // earlier version
  Serial.printf("But if we pretend like we're running version 0.0.0, we SHOULD "
                "see an update happen.\n");
  ret = ota.CheckForOTAUpdate(jsonUrl, "0.0.0");
  Serial.printf("(If the update succeeds, the reboot should prevent us ever "
                "getting here.)\n");
  Serial.printf("CheckOTAForUpdate returned %d (%s)\n\n", ret, errtext(ret));
#else
  Serial.printf("\n\ncontinuing to run existing version\n\n");
#endif
}

void loop() {}

const char *errtext(int code) {
  switch (code) {
  case ESP32OTAPull::UPDATE_AVAILABLE:
    return "An update is available but wasn't installed";
  case ESP32OTAPull::NO_UPDATE_PROFILE_FOUND:
    return "No profile matches";
  case ESP32OTAPull::NO_UPDATE_AVAILABLE:
    return "Profile matched, but update not applicable";
  case ESP32OTAPull::UPDATE_OK:
    return "An update was done, but no reboot";
  case ESP32OTAPull::HTTP_FAILED:
    return "HTTP GET failure";
  case ESP32OTAPull::WRITE_ERROR:
    return "Write error";
  case ESP32OTAPull::JSON_PROBLEM:
    return "Invalid JSON";
  case ESP32OTAPull::OTA_UPDATE_FAIL:
    return "Update fail (no OTA partition?)";
  default:
    if (code > 0)
      return "Unexpected HTTP response code";
    break;
  }
  return "Unknown error";
}

void DisplayInfo() {
  char exampleImageURL[256];
  snprintf(exampleImageURL, sizeof(exampleImageURL),
           "https://example.com/Basic-OTA-Example-%s-%s.bin", OTA_PULL_BOARD,
           CurrentVersion);

  Serial.printf("Basic-OTA-Example v%s\n", CurrentVersion);
  Serial.printf("You need to post a JSON (text) file similar to this:\n");
  Serial.printf("{\n");
  Serial.printf("  \"Configurations\": [\n");
  Serial.printf("    {\n");
  Serial.printf("      \"Board\": \"%s\",\n", OTA_PULL_BOARD);
  Serial.printf("      \"Device\": \"%s\",\n", WiFi.macAddress().c_str());
  Serial.printf("      \"Version\": %s,\n", CurrentVersion);
  Serial.printf("      \"URL\": \"%s\"\n", exampleImageURL);
  Serial.printf("    }\n");
  Serial.printf("  ]\n");
  Serial.printf("}\n");
  Serial.printf("\n");
  Serial.printf("(Board, Device, Config, and Version are all *optional*.)\n");
  Serial.printf("\n");
  Serial.printf("Post the JSON at, e.g., %s\n", jsonUrl);
  Serial.printf("Post the compiled bin at, e.g., %s\n\n", exampleImageURL);
}

void callback(int offset, int totallength) {
  Serial.printf("Updating %d of %d (%02d%%)...\n", offset, totallength,
                100 * offset / totallength);
#if defined(LED_BUILTIN) // flicker LED on update
  static int status = LOW;
  status = status == LOW && offset < totallength ? HIGH : LOW;
  digitalWrite(LED_BUILTIN, status);
#endif
}
