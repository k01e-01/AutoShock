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

    int hitIndex = -1;

    for (int i = 0; i < m_routes.size(); ++i) {
      if (rqfirstline.at(0) == m_routes.at(i).method && rqfirstline.at(1) == m_routes.at(i).url) {
        hitIndex = i;
        break;
      }
    }

    if (hitIndex == -1) {
      client->write(_404);
      client->close();
      return;
    }

    std::vector<StringView> requestHeaders;

    uint32_t contentLength;

    for (int i = 1; i < rqlines.size(); ++i) {
      if (rqlines.at(i).beforeDelimiter(' ').toString() == "Content-Length:") {
        contentLength = std::stoi(rqlines.at(i).afterDelimiter(' ').toString());
        ESP_LOGI(TAG, "HIT");
      }

      if (rqlines.at(i).length() == 0) {
        break;
      }
      requestHeaders.push_back(rqlines.at(i));
    }

    std::vector<StringView> rqparas = request.split("\r\n\r\n");
    std::string rqbdystr = "";

    if (rqparas.size() > 1) {
      for (int i = 1; i < rqparas.size(); ++i) {
        rqbdystr.append(rqparas.at(i).toString().c_str());
      }
    }

    if (rqbdystr.length() > contentLength) {
      rqbdystr = rqbdystr.substr(0, contentLength);
    }

    StringView requestBody = StringView(rqbdystr);

    CrudeHTTPServer::Response response = m_routes.at(hitIndex).cb(requestHeaders, requestBody, client);

    const std::string pleaseWork = response.body.toString();

    std::string tempStr;

    tempStr = (
      "HTTP/1.1 " +
      std::to_string(response.code) + " " +
      _HTTP_CODES.at(response.code) + _NEWLINE +

      "Server: CrudeHTTPServer" + _NEWLINE +
      "Sins-Committed: Many" + _NEWLINE +
      "Access-Control-Allow-Origin: *" + _NEWLINE
    );

    client->add(tempStr.c_str(), tempStr.length());

    bool resHasBody = response.body.length() > 0;

    if (resHasBody) {
      int tempInt = response.body.toString().length() + 2;
      tempStr = "Content-Length: " + std::to_string(tempInt) + _NEWLINE;
      client->add(tempStr.c_str(), tempStr.length());
    } else {
      client->add("Content-Length: 0\r\n", 20);
    }

    for (int i = 0; i < response.headers.size(); ++i) {
      client->add(response.headers.at(i), response.headers.at(i).length());
      client->add(_NEWLINE, 2);
    }

    std::string newTempStr;

    if (resHasBody) {
      newTempStr = (
        _NEWLINE +
        pleaseWork +
        _NEWLINE
      );

      client->add(newTempStr.c_str(), newTempStr.length());
    }

    // ESP_LOGD(TAG, "%s", newTempStr.c_str());
    // ESP_LOGD(TAG, "%s", response.body.toString().c_str());

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

  m_routes.push_back(newRoute);
  return true;
}
