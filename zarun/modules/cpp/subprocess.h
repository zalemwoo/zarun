/*
 * subprocess.h
 *
 */

#ifndef ZARUN_MODULES_SUBPROCESS_H_
#define ZARUN_MODULES_SUBPROCESS_H_

#include "base/memory/scoped_ptr.h"
#include "v8/include/v8.h"
#include "zarun/modules/native_module.h"

namespace zarun {

DECLARE_THIN_MODULE(SubProcessNative)
DECLARE_THIN_MODULE_START(SubProcessNative, "subprocess")
DECLARE_THIN_MODULE_END

}  // namespace zarun

#endif  // ZARUN_MODULES_SUBPROCESS_H_
