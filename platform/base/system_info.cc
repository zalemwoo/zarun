/*
 * system_info.cc
 *
 */

#include "platform/base/system_info.h"

#include "build/build_config.h"
#include "base/sys_info.h"

namespace zarun {
namespace platform {

namespace {

#if defined(OS_MACOSX)
#define PLATFORM "darwin"
#elif defined(OS_IOS)
#define PLATFORM "ios"
#elif defined(OS_LINUX)
#define PLATFORM "linux"
#elif defined(OS_FREEBSD)
#define PLATFORM "freebsd"
#elif defined(OS_ANDROID)
#define PLATFORM "android"
#elif defined(OS_WIN)
#define PLATFORM "windows"
#else
#error unknown platform
#endif

#if defined(ARCH_CPU_X86_FAMILY)
#if defined(ARCH_CPU_X86_64)
#define ARCH "x86_64"
#elif defined(ARCH_CPU_X86)
#define ARCH "x86"
#else
#error unknown arch
#endif
#elif defined(ARCH_CPU_ARM_FAMILY)
#define ARCH "arm"
#else
#error unknown architecture
#endif

}  // namespace

// static
std::string SystemInfo::OperatingSystemCodeName() {
  return PLATFORM;
}

// static
std::string SystemInfo::PlatformArchitecture() {
  return ARCH;
}
}
}  // namespace zarun::platform
