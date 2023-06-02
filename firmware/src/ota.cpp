#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <LittleFS.h>

#include "ota.h"

// Function to compare semver versions and determine upgrade type
int compareSemver(const char* version1, const char* version2) {
  // Remove 'v' prefix if present
  if (version1[0] == 'v') {
    version1++;
  }
  if (version2[0] == 'v') {
    version2++;
  }
  
  // Parse major, minor, and patch versions
  int major1, minor1, patch1;
  int major2, minor2, patch2;
  sscanf(version1, "%d.%d.%d", &major1, &minor1, &patch1);
  sscanf(version2, "%d.%d.%d", &major2, &minor2, &patch2);

  // Compare major, minor, and patch versions
  if (major1 < major2) {
    return 1; // Major upgrade
  } else if (major1 == major2) {
    if (minor1 < minor2) {
      return 2; // Minor upgrade
    } else if (minor1 == minor2) {
      if (patch1 < patch2) {
        return 3; // Patch upgrade
      } else if (patch1 == patch2) {
        return 0; // No upgrade
      }
    }
  }

  return -1; // Invalid or downgraded version
}

bool ota_download_and_apply_update(const char* url, BinaryType type, bool restart) {
    HTTPClient https;
    https.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

    if (url == NULL) {
        Serial.println("Invalid hostname or url");
        return false;
    }

    if (https.begin(httpsClient, url)) {
        Serial.printf("GET %s\n", url);
        int httpCode = https.GET();

        if (httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("GET %s responded with %d\n", url, httpCode);

            if (httpCode == HTTP_CODE_OK) {
                int len = https.getSize();
                int left = https.getSize();
                size_t written = 0;
                float lastProgress = 0;
                uint8_t buff[1024] = { 0 };
                if (!Update.begin(len, type == TYPE_FRONTEND ? U_SPIFFS : U_FLASH)) {
                    Update.printError(Serial);
                    https.end();
                    return false;
                }
                WiFiClient * stream = https.getStreamPtr();
                while (https.connected() && (left > 0 || left == -1)) {
                    size_t size = stream->available();
                    if(size) {
                        int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

                        if (Update.write(buff, c) != c) {
                            Update.printError(Serial);
                            https.end();
                            return false;
                        } else {
                            written += c;
                            float progress = (float)written / (float)len * 100;
                            if (lastProgress == 0 || progress - lastProgress > 1 || lastProgress >= 100) {
                                Serial.printf("Progress: %d%%\n", (int)progress);
                                lastProgress = progress;
                            }
                            if (left > 0) {
                                left -= c;
                            }
                        }
                    }
                    delay(1);
                }
                Serial.println("Progress: 100%%");
                Serial.println("Download complete");
                if (Update.end()) {
                    Serial.printf("Update Success: %u\n", written);
                    delay(100);
                    https.end();
                    if (restart) {
                      Serial.println("Restarting...");
                      ESP.restart();
                    }
                    return true;
                } else {
                    Update.printError(Serial);
                    https.end();
                    return false;
                }
            } else {
                Serial.println("Unexpected HTTP code");
                https.end();
                return false;
            }
        } else {
            Serial.printf("GET %s failed, error: %s\n", url, https.errorToString(httpCode).c_str());
            https.end();
            return false;
        }

        https.end();
    } else {
        Serial.printf("Unable to connect to %s\n", url);
        return false;
    }
    return true;
}

void ota_setup(AsyncWebServer* server, CaptivePortal* captivePortal, PID* pidController) {
    // We handle mDNS ourselves
  ArduinoOTA.setMdnsEnabled(false);

  ArduinoOTA
    .onStart([server, captivePortal, pidController]() {
      String type;

      server->end();
      captivePortal->end();
      pidController->end();
      digitalWrite(LIGHT_PIN, LOW);
      digitalWrite(HEATER_PIN, LOW);

      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      }
      else if (ArduinoOTA.getCommand() == U_SPIFFS) {
        type = "filesystem";
        LittleFS.end();
      } else {
        Serial.printf("OTA Error: Unknown command: %d\n", ArduinoOTA.getCommand());
        ESP.restart();
      }
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nOTA End");
      ESP.restart();
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("OTA Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
      ESP.restart();
    });
}

void ota_check_for_update() {
  // Check GitHub API for latest release (https://api.github.com/repos/USA-RedDragon/SnekSafe/releases/latest)
  // Compare the first 7 character of the git commit hash (target_commitish)
  // to the current commit hash GIT_COMMIT to determine if an update is available
  // If an update is available, check the assets for the correct binary (SnekSafe-esp32-<VERSION>-frontend.bin or SnekSafe-esp32-<VERSION>-firmware.bin)
  // If the binary is found, download it and apply the update with: downloadAndApplyUpdate(asset.browser_download_url, TYPE_FRONTEND);
  HTTPClient https;

  if (https.begin(httpsClient, "https://api.github.com/repos/USA-RedDragon/SnekSafe/releases/latest")) {
    Serial.println("GET https://api.github.com/repos/USA-RedDragon/SnekSafe/releases/latest");
    int httpCode = https.GET();

    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("GET https://api.github.com/repos/USA-RedDragon/SnekSafe/releases/latest responded with %d\n", httpCode);

      if (httpCode == HTTP_CODE_OK) {
        DynamicJsonDocument doc(4096);
        deserializeJson(doc, https.getString());

        const char* target_commitish = doc["target_commitish"];
        Serial.printf("target_commitish: %s\n", target_commitish);

        if (strncmp(target_commitish, GIT_COMMIT, 7) != 0) {
          Serial.println("Different git commit");
          bool frontendFound = false;
          bool firmwareFound = false;
          String frontendUrl;
          String firmwareUrl;
          for (JsonVariant asset : doc["assets"].as<JsonArray>()) {
            String name = asset["name"];

            // Check if the asset is the frontend or firmware
            if (name.startsWith("SnekSafe-esp32-") && name.endsWith("-frontend.bin")) {
              Serial.println("Found frontend update");
              String url = asset["browser_download_url"];
              Serial.printf("url: %s\n", url.c_str());

              String version = name.substring(15, name.length() - 13);
              Serial.printf("version: %s\n", version.c_str());

              // Check if the frontend is newer than the current VERSION
              // versions use semantic versioning: <major>.<minor>.<patch>
              switch (compareSemver(VERSION, version.c_str())) {
                case 1:
                  Serial.println("Major upgrade, not auto-updating");
                  break;
                case 2:
                  Serial.println("Minor upgrade, updating");
                  frontendFound = true;
                  break;
                case 3:
                  Serial.println("Patch upgrade, updating");
                  frontendFound = true;
                  break;
                case 0:
                  Serial.println("Git commit different, but version is the same, updating");
                  frontendFound = true;
                  break;
                case -1:
                  Serial.println("Invalid or downgraded version");
                  break;
                default:
                  Serial.println("Unknown upgrade type");
                  break;
              }
              if (frontendFound) {
                frontendUrl = url;
              }
            } else if (name.startsWith("SnekSafe-esp32-") && name.endsWith("-firmware.bin")) {
              Serial.println("Found firmware update");
              String url = asset["browser_download_url"];
              Serial.printf("url: %s\n", url.c_str());

              String version = name.substring(15, name.length() - 13);
              Serial.printf("version: %s\n", version.c_str());

              // Check if the frontend is newer than the current VERSION
              // versions use semantic versioning: <major>.<minor>.<patch>
              switch (compareSemver(VERSION, version.c_str())) {
                case 1:
                  Serial.println("Major upgrade, not auto-updating");
                  break;
                case 2:
                  Serial.println("Minor upgrade, updating");
                  firmwareFound = true;
                  break;
                case 3:
                  Serial.println("Patch upgrade, updating");
                  firmwareFound = true;
                  break;
                case 0:
                  Serial.println("Git commit different, but version is the same, updating");
                  firmwareFound = true;
                  break;
                case -1:
                  Serial.println("Invalid or downgraded version");
                  break;
                default:
                  Serial.println("Unknown upgrade type");
                  break;
              }
              if (firmwareFound) {
                firmwareUrl = url;
              }
            }
          }

          if (frontendFound) {
            server.end();
            if (!ota_download_and_apply_update(frontendUrl.c_str(), TYPE_FRONTEND, !firmwareFound)) {
              Serial.println("Failed to update frontend, restarting");
              ESP.restart();
            }
          }
          if (firmwareFound) {
            if (!ota_download_and_apply_update(firmwareUrl.c_str(), TYPE_FIRMWARE)) {
              Serial.println("Failed to update firmware, restarting");
              ESP.restart();
            } else {
              Serial.println("Firmware updated, restarting");
              ESP.restart();
            }
          }
        } else {
          Serial.println("No update available");
        }
      } else {
        Serial.println("Unexpected HTTP code");
      }
    } else {
      Serial.printf("GET https://api.github.com/repos/USA-RedDragon/SnekSafe/releases/latest failed, error: %s\n", https.errorToString(httpCode).c_str());
    }

    https.end();
  } else {
    Serial.printf("Unable to connect to https://api.github.com/repos/USA-RedDragon/SnekSafe/releases/latest\n");
  }
}
