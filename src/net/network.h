#ifndef CHRISTMASLIGHTS_NETWORK_H
#define CHRISTMASLIGHTS_NETWORK_H

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESPAsyncWebServer.h>     //Local WebServer used to serve the configuration portal
#include <ESPAsyncWiFiManager.h>   //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <ESP8266mDNS.h>

namespace net {
    typedef std::function<void (uint8_t * payload, size_t length)> IncomingMessage;

    void wifiSetup();

    void otaSetup();
    void otaLoop();
}

#endif