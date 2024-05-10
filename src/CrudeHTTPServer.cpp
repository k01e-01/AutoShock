#include "CrudeHTTPServer.h"

#include "Logging.h"
#include "config/Config.h"
#include "StringView.h"
#include "AsyncTCP.h"

#include <vector>
#include <tuple>
#include <unordered_map>

const char* const TAG = "CrudeHTTPServer";

using namespace OpenShock;

AsyncServer s_TCPServer(23456);

struct _Route {
  StringView url;
  StringView method;
  CrudeHTTPServer::RequestHandler cb;
};

std::vector<_Route> m_routes;

const char* const _404 = "HTTP/1.1 404 Not Found\r\nServer: CrudeHTTPServer\r\nContent-Type: text/plain\r\nContent-Length: 17\r\n\r\n404 - Not Found\r\n";

const char* const _NEWLINE = "\r\n";

const std::unordered_map<uint16_t, std::string> _HTTP_CODES = {
  {200, "OK"},
  {400, "Bad Request"},
  {403, "Forbidden"},
  {404, "Not Found"},
  {418, "I'm a teapot"},
  {420, "Enhance Your Calm"},
  {500, "Internal Server Error"},
  {501, "Not Implemented"}
};

void _handleClient(void* arg, AsyncClient* client) {
  client->onData([](void* arg, AsyncClient* client, void* data, size_t len){
    ESP_LOGI(TAG, "Client connection caught");

    StringView request = (char*)data;
    std::vector<StringView> rqlines = request.split(_NEWLINE);
    std::vector<StringView> rqfirstline = rqlines.at(0).split(' ');

    CrudeHTTPServer::Response response;
    bool routeFound = false;

    for (int i = 0; i < m_routes.size(); ++i) {
      if (rqfirstline.at(0) == m_routes.at(i).method && rqfirstline.at(1) == m_routes.at(i).url) {
        response = m_routes.at(i).cb(client);
        routeFound = true;
        break;
      }
    }

    if (!routeFound) {
      client->write(_404);
      client->close();
      return;
    }



    std::string tempStr;

    tempStr = (
      "HTTP/1.1 " +
      std::to_string(response.code) + " " +
      _HTTP_CODES.at(response.code) + _NEWLINE +

      "Server: CrudeHTTPServer" + _NEWLINE +
      "Sins-Committed: Many" + _NEWLINE +
      "Access-Control-Allow-Origin: *" + _NEWLINE +
      ""
    );

    client->add(tempStr.c_str(), tempStr.length());

    bool resHasBody = response.body.length() > 0;

    if (resHasBody) {
      tempStr = "Content-Length: " + std::to_string(response.body.length() + 2) + _NEWLINE;
      client->add(tempStr.c_str(), tempStr.length());
    } else {
      client->add("Content-Length: 0\r\n", 20);
    }

    for (int i = 0; i < response.headers.size(); ++i) {
      client->add(response.headers.at(i), response.headers.at(i).length());
      client->add(_NEWLINE, 2);
    }

    if (resHasBody) {
      tempStr = (
        _NEWLINE +
        response.body.toString() +
        _NEWLINE
      );

      client->add(tempStr.c_str(), tempStr.length());
    }

    client->send();
    client->close();

    return;

  });

  client->onDisconnect([](void* arg, AsyncClient* client){
    client->free();
  });
}

bool CrudeHTTPServer::Start() {
  ESP_LOGI(TAG, "Starting http server");
  s_TCPServer.onClient(_handleClient, NULL);
  s_TCPServer.setNoDelay(false);
  s_TCPServer.begin();
  return true;
}

bool CrudeHTTPServer::Stop() {
  ESP_LOGI(TAG, "Stopping http server");
  s_TCPServer.end();
  return true;
}

bool CrudeHTTPServer::On(const StringView& url, const StringView& method, RequestHandler cb) {
  _Route newRoute { url, method, cb };

  if (!_HTTP_CODES.count(method)) {
    ESP_LOGE(TAG, "CrudeHTTPServer does not support http status code ", method);
    return false;
  }

  m_routes.push_back(newRoute);
  return true;
}
