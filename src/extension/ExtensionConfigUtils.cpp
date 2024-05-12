#include <extension/ExtensionConfigUtils.h>

#include <Logging.h>
#include <config/Config.h>

const char* const TAG = "ExtensionConfigUtils";

using namespace OpenShock;

cJSON* ExtensionConfigUtils::GetModuleConfig(char* moduleName) {
  std::string plaintext;
  Config::GetExtensionModulesConfig(plaintext);
  cJSON* allModulesConfig = cJSON_Parse(plaintext.c_str());
  cJSON* moduleConfig = cJSON_GetObjectItemCaseSensitive(allModulesConfig, moduleName);
  cJSON_Delete(allModulesConfig);
  return moduleConfig;
}
