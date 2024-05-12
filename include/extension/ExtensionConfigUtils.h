#pragma once

#include "cJSON.h"

namespace OpenShock::ExtensionConfigUtils {
  cJSON* GetModuleConfig(char* moduleName);
} // namespace OpenShock::ExtensionConfigUtils
