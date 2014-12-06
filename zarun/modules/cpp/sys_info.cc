/*
 * sys_info.cc
 *
 */

#include "zarun/modules/cpp/sys_info.h"

#include <string>

#include "base/bind.h"
#include "base/files/file_path.h"
#include "base/sys_info.h"
#include "gin/converter.h"
#include "gin/arguments.h"

namespace zarun {

namespace {

int Uptime() {
  return static_cast<int>(base::SysInfo::Uptime());
}

}  // namespace

DEFINE_WRAPPER_INFO(SysinfoNative);

SysinfoNative::SysinfoNative(ScriptContext* context)
    : ThinNativeModule<SysinfoNative>(context) {
}

SysinfoNative::~SysinfoNative() {
}

gin::ObjectTemplateBuilder SysinfoNative::GetObjectTemplateBuilder(
    v8::Isolate* isolate) {
  return ThinNativeModule<SysinfoNative>::GetObjectTemplateBuilder(isolate)
      .SetValue("cpus", base::SysInfo::NumberOfProcessors())
      .SetValue("physMemMB", base::SysInfo::AmountOfPhysicalMemoryMB())
      .SetProperty("virtMemMB", base::SysInfo::AmountOfVirtualMemoryMB)
      .SetProperty("uptime", Uptime)
      .SetValue("hwModel", base::SysInfo::HardwareModelName())
      .SetValue("osName", base::SysInfo::OperatingSystemName())
      .SetValue("osVersion", base::SysInfo::OperatingSystemVersion())
      .SetValue("osArch", base::SysInfo::OperatingSystemArchitecture())
      .SetValue("cpuModel", base::SysInfo::CPUModelName())
      .SetValue("isLowend", base::SysInfo::IsLowEndDevice());
}

void SysinfoNative::Invalidate() {
  ThinNativeModule<SysinfoNative>::Invalidate();
}

}  // namespace zarun
