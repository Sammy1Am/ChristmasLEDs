#include "network.h"
#include "user_interface.h"

namespace net {

    const char *OTAName = "ESP8266";           // A name and a password for the OTA service
    const char *OTAPassword = "flashlight";

    void resetStoredWifi(AsyncWiFiManager *myWiFiManager) {
        system_restore(); // This wipes WiFi information (among other things), and seems to be needed (see below)
    }

    void wifiSetup() {

        // Use AsyncWiFiManager to make sure we're connected to WiFi
        AsyncWebServer server(80);
        DNSServer dns;
        AsyncWiFiManager wifiManager(&server,&dns);

        wifiManager.setAPCallback(resetStoredWifi); // If previous information fails, this needs to be called before going into AP mode (for some reason)
        wifiManager.autoConnect("HouseLights","h0usel1ghts");

        if (!MDNS.begin("houselights")) {             // Start the mDNS responder for esp8266.local
            Serial.println("Error setting up MDNS responder!");
        }
        Serial.println("mDNS responder started");
    }

    void otaSetup() {

        ArduinoOTA.setPort(8377);
        ArduinoOTA.setPassword("flashlight");

        ArduinoOTA.onStart([]() {
            Serial.println("OTA Start");
        });
        ArduinoOTA.onEnd([]() {
            Serial.println("\nOTA End");
        });
        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
            Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        });
        ArduinoOTA.onError([](ota_error_t error) {
            Serial.printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
            else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
            else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
            else if (error == OTA_END_ERROR) Serial.println("End Failed");
        });
        ArduinoOTA.begin();
        Serial.println("OTA Ready");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    }

    void otaLoop() {
        ArduinoOTA.handle();
    }
}