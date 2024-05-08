#pragma once

#include "config/ConfigBase.h"
#include "StringView.h"

#include <string>

namespace OpenShock::Config {
  struct ExtensionConfig : public ConfigBase<Serialization::Configuration::ExtensionConfig> {
    ExtensionConfig();
    ExtensionConfig(StringView ptModulesConfig);

    std::string ptModulesConfig;

    void ToDefault() override;

    bool FromFlatbuffers(const Serialization::Configuration::ExtensionConfig* config) override;
    flatbuffers::Offset<Serialization::Configuration::ExtensionConfig> ToFlatbuffers(flatbuffers::FlatBufferBuilder& builder, bool withSensitiveData) const override;

    bool FromJSON(const cJSON* json) override;
    cJSON* ToJSON(bool withSensitiveData) const override;
  };
}  // namespace Openshock::Config
