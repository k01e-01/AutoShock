#include <extension/ExtensionManager.h>

#include <Logging.h>
#include <ESPAsyncWebServer.h>
#include <config/Config.h>

#include <cstring>

const char* const TAG = "ExtensionManager";

using namespace OpenShock;

static bool s_armed = false;

bool ExtensionManager::IsArmed() {
  return s_armed;
}

bool ExtensionManager::Init() {
  ESP_LOGI(TAG, "Hello world from ExtensionManager!");
  return true;
}

void ExtensionManager::Update() {}
