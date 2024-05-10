#include <extension/ExtensionManager.h>

#include <Logging.h>
#include <CrudeHTTPServer.h>
#include <config/Config.h>

#include <cstring>
#include <vector>

const char* const TAG = "ExtensionManager";

using namespace OpenShock;

static bool s_armed = false;

bool ExtensionManager::IsArmed() {
  return s_armed;
}

bool ExtensionManager::Init() {
  ESP_LOGI(TAG, "Hello world from ExtensionManager!");

  CrudeHTTPServer::On("/", "GET", [](AsyncClient* client){
    return CrudeHTTPServer::Response {
      200,
      std::vector<StringView> {
        "Content-Type: text/plain"
      },
      "Hello world from ExtensionManager"
    };
  });

  CrudeHTTPServer::Start();

  return true;
}

void ExtensionManager::Update() {}
