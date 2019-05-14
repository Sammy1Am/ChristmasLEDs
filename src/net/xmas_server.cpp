#include "net/xmas_server.h"

namespace net {
    AsyncWebServer awServer(80);       // Create a webserver object that listens for HTTP request on port 80
    AsyncWebSocket ws("/ws");
    IncomingMessage socketPayloadHandler;

    void startSPIFFS() { // Start the SPIFFS and list all contents
        SPIFFS.begin();                             // Start the SPI Flash File System (SPIFFS)
        Serial.println("SPIFFS started. Contents:");
        {
            Dir dir = SPIFFS.openDir("/");
            while (dir.next()) {                      // List the file system contents
            String fileName = dir.fileName();
            Serial.printf("\tFS File: %s\r\n", fileName.c_str());
            }
            Serial.printf("\n");
        }
    }

    void serverSetup(IncomingMessage payloadHandler) { // Start a HTTP server with a file read handler and an upload handler
        socketPayloadHandler = payloadHandler;

        ws.onEvent(webSocketsEvent);
        awServer.addHandler(&ws);


        startSPIFFS(); // Server needs file system (no one else needs to know)
        awServer.onNotFound(notFound);          // Serve static should take care of things, otherwise, 404
        awServer.on("/getConfig", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send(SPIFFS, "/config.js", "application/javascript", false, configProcessor);
        });
        awServer.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

        awServer.begin();                             // start the HTTP server
        Serial.println("HTTP server started.");
    }

    void notFound(AsyncWebServerRequest *request) {
        request->send(404, "text/plain", "Not found");
    }

    String configProcessor(const String& var) {
        if(var == "MODE")
            return String(config::getConfig(config::SmallSetting::MODE));
        if(var == "BRIGHTNESS")
            return String(config::getConfig(config::SmallSetting::GLOBAL_BRIGHTNESS));
        if(var == "RF_TWINKLE")
            return String(config::getConfig(config::SmallSetting::RF_TWINKLE_ON));
        if(var == "RF_BANDSIZE")
            return String(config::getConfig(config::SmallSetting::RF_BAND_SIZE));
        if(var == "XT_TWINKLE")
            return String(config::getConfig(config::SmallSetting::XT_TWINKLE_ON));
        if(var == "XT_STARSIZE")
            return String(config::getConfig(config::SmallSetting::XT_STAR_SIZE));
        if(var == "XT_ORNAMENTSPACING")
            return String(config::getConfig(config::SmallSetting::XT_ORNAMENT_SPACING));
        if(var == "HB_BPM")
            return String(config::getConfig(config::SmallSetting::HB_BPM));
        return String();
    }

    void webSocketsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *payload, size_t length) {
        Serial.println("Handling event...");
        switch(type) {
            case WS_EVT_DISCONNECT:
                Serial.printf("[%u] Disconnected!\n", client->id());
                break;
            case WS_EVT_CONNECT:
                {
                    IPAddress ip = client->remoteIP();
                    Serial.printf("[%u] Connected from %d.%d.%d.%d\n", client->id(), ip[0], ip[1], ip[2], ip[3]);
                    // send message to client
                    client->text("Connected.");
                }
                break;
            case WS_EVT_DATA:
                AwsFrameInfo * info = (AwsFrameInfo*)arg;
                if (info->opcode == WS_TEXT) {
                    Serial.printf("[%u] get Text: %s\n", client->id(), payload);
                } else {
                    Serial.printf("[%u] get binary length: %u\n", client->id(), length);
                    // Pass payload along to handler
                    socketPayloadHandler(payload, length);
                }
                break;
        }

    }
}