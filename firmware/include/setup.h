#ifndef __SETUP_H__
#define __SETUP_H__

#include <ESPAsyncWebServer.h>

class CaptiveSetupHandler : public AsyncWebHandler {
public:
  CaptiveSetupHandler();

  bool canHandle(AsyncWebServerRequest *request);

  void handleRequest(AsyncWebServerRequest *request);
  void handleHTTP(AsyncWebServerRequest *request);
};

#endif
