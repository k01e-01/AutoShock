#include "config/ExtensionConfig.h"

#include "config/internal/utils.h"
#include "Logging.h"

const char* const TAG = "Config::ExtensionConfig";

using namespace OpenShock::Config;

ExtensionConfig::ExtensionConfig() : ptExtensionsConfig() { }

ExtensionConfig::ExtensionConfig(StringView ptExtensionsConfig) : ptExtensionsConfig(ptExtensionsConfig.toString()) { }

void ExtensionConfig::ToDefault() {
  ptExtensionsConfig.clear();
}

bool ExtensionConfig::FromFlatbuffers(const Serialization::Configuration::ExtensionConfig* config) {
  if (config == nullptr) {
    ESP_LOGE(TAG, "config is null");
    return false;
  }

  Internal::Utils::FromFbsStr(ptExtensionsConfig, config->pt_extensions_config(), "");

  return true;
}

flatbuffers::Offset<OpenShock::Serialization::Configuration::ExtensionConfig> ExtensionConfig::ToFlatbuffers(flatbuffers::FlatBufferBuilder& builder, bool withSensitiveData) const {
  auto ptExtensionsConfigOffset = builder.CreateString(ptExtensionsConfig);

  return Serialization::Configuration::CreateExtensionConfig(builder, ptExtensionsConfigOffset);
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

  Internal::Utils::FromJsonStr(ptExtensionsConfig, json, "ptExtensionsConfig", "");

  return true;
}

cJSON* ExtensionConfig::ToJSON(bool withSensitive) const {
  cJSON* root = cJSON_CreateObject();

  cJSON_AddStringToObject(root, "ptExtensionsConfig", ptExtensionsConfig.c_str());

  return root;
}
