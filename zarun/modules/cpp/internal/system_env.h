/*
 * env.h
 *
 */

#ifndef ZARUN_MODULES_CPP_INTERNAL_SYSTEM_ENV_H_
#define ZARUN_MODULES_CPP_INTERNAL_SYSTEM_ENV_H_

#include "v8/include/v8.h"

namespace zarun {
namespace internal {

v8::Handle<v8::Object> CreateSystemEnv(v8::Isolate* isolate);
}
}  // namespace zarun::internal

#endif  // ZARUN_MODULES_CPP_INTERNAL_SYSTEM_ENV_H_
