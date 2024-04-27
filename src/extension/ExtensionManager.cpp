#include <extension/ExtensionManager.h>

#include <Logging.h>
#include <config/Config.h>

#include <Arduino.h>

const char* const TAG = "ExtensionManager";

using namespace OpenShock;

bool ExtensionManager::Init() {
  ESP_LOGI(TAG, "Hello world from ExtensionManager!");
  return true;
}

void ExtensionManager::Update() {
  return;
}
