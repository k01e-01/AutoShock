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
  typedef std::function<Response(std::vector<StringView>,StringView,AsyncClient*)> RequestHandler;
  bool Start();
  bool Stop();
  bool On(const StringView& url, const StringView& method, RequestHandler cb);
} // namespace OpenShock::CrudeHTTPServer
