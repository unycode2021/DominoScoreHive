#include <DNSServer.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include "ESPAsyncWebServer.h"
#include "scoreboard_controller.h"
#include <ArduinoJson.h>
#include "../data/index.h"

extern bool isConfigured;
// Global state management

extern DNSServer dnsServer;
extern AsyncWebServer server;

class CaptiveRequestHandler : public AsyncWebHandler
{
public:
    CaptiveRequestHandler() {}
    virtual ~CaptiveRequestHandler() {}

    bool canHandle(AsyncWebServerRequest *request)
    {
        // Only handle root and captive portal requests
        String host = request->host();
        return !request->url().startsWith("/api/") &&
               !request->url().startsWith("/setup") &&
               !request->url().startsWith("/scores") &&
               !request->url().startsWith("/players") &&
               !request->url().startsWith("/config");
    }

    void handleRequest(AsyncWebServerRequest *request)
    {
        AsyncResponseStream *response = request->beginResponseStream("text/html");
        response->print(PAGE_INDEX);
        request->send(response);
    }
};