#pragma once

#include "freertos/FreeRTOS.h"
#include "AsyncTCP.h"
#include "StringView.h"

#include "vector"

namespace OpenShock::CrudeHTTPServer {
  struct Response {
    uint16_t code;
    std::vector<StringView> headers;
    StringView body;
  };
  typedef std::function<Response(AsyncClient*)> RequestHandler;
  bool Start();
  bool Stop();
  void On(const StringView& url, const StringView& method, RequestHandler cb);
}
