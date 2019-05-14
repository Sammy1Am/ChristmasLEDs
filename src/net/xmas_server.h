#ifndef CHRISTMASLIGHTS_NET_SERVER_H
#define CHRISTMASLIGHTS_NET_SERVER_H

#include <ESPAsyncWebServer.h>
#include <FS.h>
#include "config/config.h"

namespace net {
    typedef std::function<void (uint8_t * payload, size_t length)> IncomingMessage;

    void startSPIFFS();
    void serverSetup(IncomingMessage messageHandler);
    void notFound(AsyncWebServerRequest *request);
    String configProcessor(const String& var);
    void webSocketsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *payload, size_t length);
}

#endif