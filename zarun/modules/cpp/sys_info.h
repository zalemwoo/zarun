/*
 * sys_info.h
 *
 */

#ifndef ZARUN_MODULES_SYS_INFO_H_
#define ZARUN_MODULES_SYS_INFO_H_

#include "v8/include/v8.h"
#include "zarun/modules/native_module.h"

namespace zarun {

DECLARE_THIN_MODULE(SysinfoNative)
DECLARE_THIN_MODULE_START(SysinfoNative, "sys_info")
DECLARE_THIN_MODULE_END

}  // namespace zarun

#endif  // ZARUN_MODULES_SYS_INFO_H_
