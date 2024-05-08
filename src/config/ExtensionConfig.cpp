#include "config/ExtensionConfig.h"

#include "config/internal/utils.h"
#include "Logging.h"

const char* const TAG = "Config::ExtensionConfig";

using namespace OpenShock::Config;

ExtensionConfig::ExtensionConfig() : ptModulesConfig() { }

ExtensionConfig::ExtensionConfig(StringView ptModulesConfig) : ptModulesConfig(ptModulesConfig.toString()) { }

void ExtensionConfig::ToDefault() {
  ptModulesConfig.clear();
}

bool ExtensionConfig::FromFlatbuffers(const Serialization::Configuration::ExtensionConfig* config) {
  if (config == nullptr) {
    ESP_LOGE(TAG, "config is null");
    return false;
  }

  Internal::Utils::FromFbsStr(ptModulesConfig, config->pt_modules_config(), "");

  return true;
}

flatbuffers::Offset<OpenShock::Serialization::Configuration::ExtensionConfig> ExtensionConfig::ToFlatbuffers(flatbuffers::FlatBufferBuilder& builder, bool withSensitiveData) const {
  auto ptModulesConfigOffset = builder.CreateString(ptModulesConfig);

  return Serialization::Configuration::CreateExtensionConfig(builder, ptModulesConfigOffset);
}

bool ExtensionConfig::FromJSON(const cJSON* json) {
  if (json == nullptr) {
    ESP_LOGE(TAG, "json is null");
    return false;
  }

  if (cJSON_IsObject(json) == 0) {
    ESP_LOGE(TAG, "json is not an object");
    return false;
  }

  Internal::Utils::FromJsonStr(ptModulesConfig, json, "ptModulesConfig", "");

  return true;
}

cJSON* ExtensionConfig::ToJSON(bool withSensitive) const {
  cJSON* root = cJSON_CreateObject();

  cJSON_AddStringToObject(root, "ptModulesConfig", ptModulesConfig.c_str());

  return root;
}
