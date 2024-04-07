#include <Arduino.h>
#include <ArduinoJson.h>
#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <Updater.h>
#include <stdlib_noniso.h>

#include "dbglog.h"

enum class OTAMode {
    firmware,
    filesystem
};

DNSServer dnsServer;
AsyncWebServer server(80);
JsonDocument config;
OTAMode ota_mode;
bool reboot_requested = false;
unsigned long reboot_request_millis;

void setup_serial() {
    Serial.begin(76800);
    Serial.println();
}

void setup_config() {
    // Init file system
    if (LittleFS.begin()) return;
    DBGLOG_WARNING("Warning: File system not formated.")

    // Format file system
    if (!LittleFS.format())
        DBGLOG_CRITICAL("Critical error: File system format failed.");

    // Init config.json
    auto file = LittleFS.open("config.json", "w");
    config["user"]["name"] = "admin";
    config["user"]["pass"] = "12345";
    serializeJson(config, file);
    file.close();

    DBGLOG_INFO("Info: Config initialized.");
}

void setup_wifi_access_point() {
    String mac = WiFi.macAddress();
    String ssid = "ScreenRotator_" + mac.substring(12, 14) + mac.substring(15, 17);
    if (WiFi.softAP(ssid, "", 1, false, 4)) {
        DBGLOG_INFO("Info: Access Point is Creadted with SSID:  '%s'\nAccess Point IP: %s", ssid.c_str(), WiFi.softAPIP().toString().c_str());
        dnsServer.start(53, "*", WiFi.softAPIP());
    } else {
        DBGLOG_CRITICAL("Critical error: Unable to create access point.");
    }
}

void setup_wifi_station() {
    DBGLOG_INFO("Info: connecting to '%s'...", config["wifi"]["ssid"].as<const char *>());
    for (uint i = 20; i; i--) {
        if (WiFi.begin(config["wifi"]["ssid"].as<const char *>(), config["wifi"]["pass"].as<const char *>()) == WL_CONNECTED) {
            DBGLOG_INFO("Info: Connected.\nIP Address: %s", WiFi.localIP().toString().c_str());
            return;
        }
        delay(1000);
    }

    DBGLOG_WARNING("Warning: Unable to connect.");
    setup_wifi_access_point();
}

void setup_wifi() {
    if (config.containsKey("wifi") && config["wifi"].containsKey("ssid") && config["wifi"].containsKey("pass")) {
        setup_wifi_station();
    } else {
        setup_wifi_access_point();
    }
}

void setup_http() {
    auto user = config["user"]["name"].as<const char *>();
    auto pass = config["user"]["pass"].as<const char *>();

    server.on("/ota/start", HTTP_GET, [&](AsyncWebServerRequest *request) {
        if (!request->authenticate(user, pass))
            return request->requestAuthentication();

        // Get ota mode value, if present
        ota_mode = OTAMode::firmware;
        if (request->hasParam("mode")) {
            if (request->getParam("mode")->value() == "fs") {
                ota_mode = OTAMode::filesystem;
                DBGLOG_INFO("Info: OTA Mode: Filesystem");
            } else {
                DBGLOG_INFO("Info: OTA Mode: Firmware");
            }
        }

        // Get file MD5 hash from arg
        if (request->hasParam("hash")) {
            auto hash = request->getParam("hash")->value();
            DBGLOG_INFO("Info: OTA hash: %s", hash);
            if (!Update.setMD5(hash.c_str())) {
                DBGLOG_ERROR("ERROR: MD5 hash not valid\n");
                return request->send(400, "text/plain", "MD5 parameter invalid");
            }
        }

        Update.runAsync(true);

        bool ok;
        if (ota_mode == OTAMode::filesystem) {
            close_all_fs();
            ok = Update.begin(FS_end - FS_start, U_FS);
        } else {
            ok = Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000, U_FLASH);
        }

        if (ok) {
            request->send(200, "text/plain", "OK");
        } else {
            const char *error = Update.getErrorString().c_str();
            request->send(400, "text/plain", error);
            DBGLOG_ERROR("Error: Failed to start update process\n %s", error);
        }
    });

    server.on( "/ota/upload", HTTP_POST, [&](AsyncWebServerRequest *request) {
        if (!request->authenticate(user, pass))
            return request->requestAuthentication();

        auto response = Update.hasError()
          ? request->beginResponse(400, "text/plain", Update.getErrorString().c_str())
          : request->beginResponse(200, "text/plain", "OK");

        response->addHeader("Connection", "close");
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);

        if (!Update.hasError()) {
            reboot_requested = true;
            reboot_request_millis = millis();
        }
    },
    [&](AsyncWebServerRequest *request, String, size_t index, uint8_t *data, size_t len, bool final) {
        if (!request->authenticate(user, pass))
            return request->requestAuthentication();

        // Reset progress size on first frame
        static size_t ota_progress_size;
        if (index == 0) ota_progress_size = 0;

        if (len) {
            if (Update.write(data, len) != len)
                return request->send(400, "text/plain", "Failed to write chunked data to free space");
            ota_progress_size += len;
        }

        if (final) Update.end(true);
    });

    server.serveStatic("/", LittleFS, "/www");
    server.begin();
}

void setup_mpu() {
    ;
}

void setup() {
    setup_serial();
    setup_config();
    setup_wifi();
    setup_http();
    setup_mpu();
}

void loop() {
    dnsServer.processNextRequest();

    if (reboot_requested && (millis() - reboot_request_millis > 2000)) {
        DBGLOG_INFO("Info: Rebooting...");
        ESP.restart();
    }
}
