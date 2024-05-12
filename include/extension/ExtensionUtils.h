#pragma once

#include "cJSON.h"

namespace OpenShock::Extensions::ExtensionUtils {
  cJSON* GetExtensionConfig(char* extensionName);
} // namespace OpenShock::ExtensionConfigUtils
