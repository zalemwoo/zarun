/*
 * system_info.h
 *
 */

#ifndef ZARUN_PLATFORM_BASE_SYSTEM_INFO_H_
#define ZARUN_PLATFORM_BASE_SYSTEM_INFO_H_

#include <string>

#include "platform/platform_export.h"

namespace zarun {
namespace platform {

class PLATFORM_EXPORT SystemInfo {
 public:
  static std::string OperatingSystemCodeName();
  static std::string PlatformArchitecture();
  static bool IsPosix();
};
}
}  // namespace zarun::platform

#endif  // ZARUN_PLATFORM_BASE_SYSTEM_INFO_H_
