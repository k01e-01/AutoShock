#include <extension/ExtensionManager.h>

#include <Logging.h>
#include <ESPAsyncWebServer.h>
#include <config/Config.h>

#include <cstring>

const char* const TAG = "ExtensionManager";

using namespace OpenShock;

AsyncWebServer ExtensionServer(23456);

static bool s_armed = false;

bool ExtensionManager::IsArmed() {
  return s_armed;
}

bool ExtensionManager::Init() {
  ESP_LOGI(TAG, "Hello world from ExtensionManager!");

  ExtensionServer.on("/echo", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Hello world from ExtensionManager!");
  });

  ExtensionServer.on("/arm", HTTP_POST, [](AsyncWebServerRequest *request){
    s_armed = true;
    request->send(200);
  });

  ExtensionServer.on("/disarm", HTTP_POST, [](AsyncWebServerRequest *request){
    s_armed = false;
    request->send(200);
  });

  ExtensionServer.on("/config", HTTP_GET, [](AsyncWebServerRequest *request){
    std::string ptModulesConfig;

    if (!Config::GetExtensionModulesConfig(ptModulesConfig)) {
      ESP_LOGE(TAG, "Failed to get ptModulesConfig");
      request->send(500, "text/plain", "Internal Server Error: failed to get config");
    }

    ESP_LOGI(TAG, "Got ptModulesConfig: ", ptModulesConfig);
    request->send(200, "text/plain", ptModulesConfig.c_str());
  });

  ExtensionServer.on("/config", HTTP_POST, [](AsyncWebServerRequest *request){
    if (!request->hasParam("data", true)) {
      request->send(400, "text/plain", "Invalid Request: no data");
      return;
    }

    AsyncWebParameter* postData = request->getParam("data", true);
    String ptModulesConfig = postData->value();
    ESP_LOGI(TAG, "Received new config from API: ", ptModulesConfig);


    if (!Config::SetExtensionModulesConfig(ptModulesConfig)) {
      ESP_LOGE(TAG, "Failed to set ptModulesConfig");
      request->send(500, "text/plain", "Internal Server Error: failed to set config");
      return;
    }

    request->send(200);
  });

  ExtensionServer.begin();
  return true;
}

void ExtensionManager::Update() {
  return;
}
