#include <extension/ExtensionUtils.h>

#include <Logging.h>
#include <config/Config.h>

const char* const TAG = "ExtensionUtils";

using namespace OpenShock::Extensions;

cJSON* ExtensionUtils::GetExtensionConfig(char* extensionName) {
  std::string plaintext;
  Config::GetExtensionExtensionsConfig(plaintext);
  cJSON* allExtensionsConfig = cJSON_Parse(plaintext.c_str());
  cJSON* extensionConfig = cJSON_GetObjectItemCaseSensitive(allExtensionsConfig, extensionName);
  cJSON_Delete(allExtensionsConfig);
  return extensionConfig;
}
