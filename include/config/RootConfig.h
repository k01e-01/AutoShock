#pragma once

#include "config/BackendConfig.h"
#include "config/CaptivePortalConfig.h"
#include "config/ConfigBase.h"
#include "config/OtaUpdateConfig.h"
#include "config/RFConfig.h"
#include "config/SerialInputConfig.h"
#include "config/WiFiConfig.h"

#include "config/ExtensionConfig.h"

namespace OpenShock::Config {
  struct RootConfig : public ConfigBase<Serialization::Configuration::Config> {
    OpenShock::Config::RFConfig rf;
    OpenShock::Config::WiFiConfig wifi;
    OpenShock::Config::CaptivePortalConfig captivePortal;
    OpenShock::Config::BackendConfig backend;
    OpenShock::Config::SerialInputConfig serialInput;
    OpenShock::Config::OtaUpdateConfig otaUpdate;

    OpenShock::Config::ExtensionConfig extension;

    void ToDefault() override;

    bool FromFlatbuffers(const Serialization::Configuration::Config* config) override;
    flatbuffers::Offset<Serialization::Configuration::Config> ToFlatbuffers(flatbuffers::FlatBufferBuilder& builder, bool withSensitiveData) const override;

    bool FromJSON(const cJSON* json) override;
    cJSON* ToJSON(bool withSensitiveData) const override;
  };
}  // namespace OpenShock::Config
