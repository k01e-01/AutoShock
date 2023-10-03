#include "CaptivePortal.h"

#include "AuthenticationManager.h"
#include "Utils/HexUtils.h"
#include "WiFiManager.h"
#include "Mappers/EspWiFiTypesMapper.h"
#include "WiFiScanManager.h"

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <WebSocketsServer.h>
#include <WiFi.h>

#include <memory>

static const char* TAG = "CaptivePortal";

constexpr std::uint16_t HTTP_PORT               = 80;
constexpr std::uint16_t WEBSOCKET_PORT          = 81;
constexpr std::uint32_t WEBSOCKET_PING_INTERVAL = 10'000;
constexpr std::uint32_t WEBSOCKET_PING_TIMEOUT  = 1000;
constexpr std::uint8_t WEBSOCKET_PING_RETRIES   = 3;

struct CaptivePortalInstance {
  CaptivePortalInstance() : webServer(HTTP_PORT), socketServer(WEBSOCKET_PORT, "/ws", "json") { }

  AsyncWebServer webServer;
  WebSocketsServer socketServer;
  OpenShock::WiFiScanManager::CallbackHandle wifiScanStartedHandlerId;
  OpenShock::WiFiScanManager::CallbackHandle wifiScanCompletedHandlerId;
  OpenShock::WiFiScanManager::CallbackHandle wifiScanDiscoveryHandlerId;
};
static std::unique_ptr<CaptivePortalInstance> s_webServices = nullptr;

void handleWebSocketEvent(std::uint8_t socketId, WStype_t type, std::uint8_t* data, std::size_t len);
void handleHttpNotFound(AsyncWebServerRequest* request);

using namespace OpenShock;

bool CaptivePortal::Start() {
  if (s_webServices != nullptr) {
    ESP_LOGD(TAG, "Already started");
    return true;
  }

  ESP_LOGD(TAG, "Starting");

  if (!WiFi.enableAP(true)) {
    ESP_LOGE(TAG, "Failed to enable AP mode");
    return false;
  }

  if (!WiFi.softAP(("OpenShock-" + WiFi.macAddress()).c_str())) {
    ESP_LOGE(TAG, "Failed to start AP");
    WiFi.enableAP(false);
    return false;
  }

  IPAddress apIP(10, 10, 10, 10);
  if (!WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0))) {
    ESP_LOGE(TAG, "Failed to configure AP");
    WiFi.softAPdisconnect(true);
    return false;
  }

  s_webServices = std::make_unique<CaptivePortalInstance>();

  s_webServices->socketServer.onEvent(handleWebSocketEvent);
  s_webServices->socketServer.begin();
  s_webServices->socketServer.enableHeartbeat(WEBSOCKET_PING_INTERVAL, WEBSOCKET_PING_TIMEOUT, WEBSOCKET_PING_RETRIES);

  s_webServices->webServer.serveStatic("/", LittleFS, "/www/").setDefaultFile("index.html");
  s_webServices->webServer.onNotFound([](AsyncWebServerRequest* request) { request->send(404, "text/plain", "Not found"); });
  s_webServices->webServer.begin();

  s_webServices->wifiScanStartedHandlerId   = WiFiScanManager::RegisterScanStartedHandler([]() {
    ESP_LOGD(TAG, "WiFi scan started");
    StaticJsonDocument<256> doc;
    doc["type"]    = "wifi";
    doc["subject"] = "scan_started";
    doc["status"]  = "ok";
    CaptivePortal::BroadcastMessageJSON(doc);
  });
  s_webServices->wifiScanCompletedHandlerId = WiFiScanManager::RegisterScanCompletedHandler([](WiFiScanManager::ScanCompletedStatus status) {
    ESP_LOGD(TAG, "WiFi scan completed");
    StaticJsonDocument<256> doc;
    doc["type"]    = "wifi";
    doc["subject"] = "scan_completed";
    doc["status"]  = "ok";
    // doc["status"] = EspWiFiTypesMapper::MapScanCompletedStatus(status);
    CaptivePortal::BroadcastMessageJSON(doc);
  });
  s_webServices->wifiScanDiscoveryHandlerId = WiFiScanManager::RegisterScanDiscoveryHandler([](const wifi_ap_record_t* record) {
    ESP_LOGD(TAG, "WiFi scan discovery");
    StaticJsonDocument<256> doc;
    doc["type"]    = "wifi";
    doc["subject"] = "scan_discovery";
    auto data      = doc.createNestedObject("data");
    data["ssid"]   = reinterpret_cast<const char*>(record->ssid);
    data["bssid"]  = HexUtils::ToHexMac<6>(record->bssid).data();
    doc["rssi"]    = record->rssi;
    doc["channel"] = record->primary;
    doc["security"] = Mappers::GetWiFiAuthModeName(record->authmode);

    CaptivePortal::BroadcastMessageJSON(doc);
  });

  ESP_LOGD(TAG, "Started");

  return true;
}
void CaptivePortal::Stop() {
  if (s_webServices == nullptr) {
    ESP_LOGD(TAG, "Already stopped");
    return;
  }

  ESP_LOGD(TAG, "Stopping");

  s_webServices->webServer.end();
  s_webServices->socketServer.close();

  WiFiScanManager::UnregisterScanStartedHandler(s_webServices->wifiScanStartedHandlerId);
  WiFiScanManager::UnregisterScanCompletedHandler(s_webServices->wifiScanCompletedHandlerId);
  WiFiScanManager::UnregisterScanDiscoveryHandler(s_webServices->wifiScanDiscoveryHandlerId);

  s_webServices = nullptr;

  WiFi.softAPdisconnect(true);
}
bool CaptivePortal::IsRunning() {
  return s_webServices != nullptr;
}
void CaptivePortal::Update() {
  if (s_webServices == nullptr) {
    return;
  }

  s_webServices->socketServer.loop();
}
bool CaptivePortal::BroadcastMessageTXT(const char* data, std::size_t len) {
  if (s_webServices == nullptr) {
    return false;
  }

  s_webServices->socketServer.broadcastTXT(data, len);

  return true;
}
bool CaptivePortal::BroadcastMessageBIN(const std::uint8_t* data, std::size_t len) {
  if (s_webServices == nullptr) {
    return false;
  }

  s_webServices->socketServer.broadcastBIN(data, len);

  return true;
}

void handleWebSocketClientConnected(std::uint8_t socketId) {
  ESP_LOGD(TAG, "WebSocket client #%u connected from %s", socketId, s_webServices->socketServer.remoteIP(socketId).toString().c_str());
}
void handleWebSocketClientDisconnected(std::uint8_t socketId) {
  ESP_LOGD(TAG, "WebSocket client #%u disconnected", socketId);
}
void handleWebSocketClientWiFiScanMessage(const StaticJsonDocument<256>& doc) {
  bool run = doc["run"];
  if (run) {
    WiFiScanManager::StartScan();
  } else {
    WiFiScanManager::CancelScan();
  }
}
void handleWebSocketClientWiFiAuthenticateMessage(const StaticJsonDocument<256>& doc) {
  String bssidStr = doc["bssid"];
  if (bssidStr.isEmpty()) {
    ESP_LOGE(TAG, "WiFi BSSID is missing");
    return;
  }
  if (bssidStr.length() != 17) {
    ESP_LOGE(TAG, "WiFi BSSID is invalid");
    return;
  }

  String password = doc["password"];

  // Convert BSSID to byte array
  std::uint8_t bssid[6];
  if (sscanf(bssidStr.c_str(), "%02X:%02X:%02X:%02X:%02X:%02X", bssid + 0, bssid + 1, bssid + 2, bssid + 3, bssid + 4, bssid + 5) != 6) {
    ESP_LOGE(TAG, "WiFi BSSID is invalid");
    return;
  }

  WiFiManager::Authenticate(bssid, password);
}
void handleWebSocketClientWiFiConnectMessage(const StaticJsonDocument<256>& doc) {
  std::uint16_t wifiId = doc["id"];

  WiFiManager::Connect(wifiId);
}
void handleWebSocketClientWiFiDisconnectMessage(const StaticJsonDocument<256>& doc) {
  WiFiManager::Disconnect();
}
void handleWebSocketClientWiFiForgetMessage(const StaticJsonDocument<256>& doc) {
  WiFiManager::Forget(doc["bssid"]);
}
void handleWebSocketClientWiFiMessage(StaticJsonDocument<256> doc) {
  // Parse "action" field
  String actionStr = doc["action"];
  if (actionStr.isEmpty()) {
    ESP_LOGE(TAG, "WiFi action is missing");
    return;
  }

  if (actionStr == "scan") {
    handleWebSocketClientWiFiScanMessage(doc);
  } else if (actionStr == "authenticate") {
    handleWebSocketClientWiFiAuthenticateMessage(doc);
  } else if (actionStr == "connect") {
    handleWebSocketClientWiFiConnectMessage(doc);
  } else if (actionStr == "disconnect") {
    handleWebSocketClientWiFiDisconnectMessage(doc);
  } else if (actionStr == "forget") {
    handleWebSocketClientWiFiForgetMessage(doc);
  } else {
    ESP_LOGE(TAG, "Unknown WiFi action: %s", actionStr.c_str());
  }
}
void handleWebSocketClientMessage(std::uint8_t socketId, WStype_t type, std::uint8_t* data, std::size_t len) {
  if (type != WStype_t::WStype_TEXT) {
    ESP_LOGE(TAG, "Message type is not supported");
    return;
  }

  StaticJsonDocument<256> doc;
  auto err = deserializeJson(doc, data, len);
  if (err) {
    ESP_LOGE(TAG, "Failed to deserialize message: %s", err.c_str());
    return;
  }

  String typeStr = doc["type"];
  if (typeStr.isEmpty()) {
    ESP_LOGE(TAG, "Message type is missing");
    return;
  }

  if (typeStr == "wifi") {
    handleWebSocketClientWiFiMessage(doc);
  } /* else if (typeStr == "connect") {
     WiFiManager::Connect(doc["ssid"], doc["password"]);
   } else if (typeStr == "disconnect") {
     WiFiManager::Disconnect();
   } else if (typeStr == "authenticate") {
     AuthenticationManager::Authenticate(doc["code"]);
   } else if (typeStr == "pair") {
     AuthenticationManager::Pair(doc["code"]);
   } else if (typeStr == "unpair") {
     AuthenticationManager::Unpair();
   } else if (typeStr == "setRmtPin") {
     AuthenticationManager::SetRmtPin(doc["pin"]);
   }*/
}
void handleWebSocketClientError(std::uint8_t socketId, std::uint16_t code, const char* message) {
  ESP_LOGE(TAG, "WebSocket client #%u error %u: %s", socketId, code, message);
}
void handleWebSocketEvent(std::uint8_t socketId, WStype_t type, std::uint8_t* data, std::size_t len) {
  switch (type) {
    case WStype_CONNECTED:
      handleWebSocketClientConnected(socketId);
      break;
    case WStype_DISCONNECTED:
      handleWebSocketClientDisconnected(socketId);
      break;
    case WStype_BIN:
    case WStype_TEXT:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      handleWebSocketClientMessage(socketId, type, data, len);
      break;
    case WStype_PING:
    case WStype_PONG:
      // Do nothing
      break;
    case WStype_ERROR:
      handleWebSocketClientError(socketId, len, reinterpret_cast<char*>(data));
      break;
    default:
      ESP_LOGE(TAG, "Unknown WebSocket event type: %d", type);
      break;
  }
}
