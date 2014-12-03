/*
 * subprocess.h
 *
 */

#ifndef ZARUN_MODULES_SUBPROCESS_H_
#define ZARUN_MODULES_SUBPROCESS_H_

#include "v8/include/v8.h"

#include "zarun/modules/object_backed_native_module.h"

namespace zarun {

class ScriptContext;

class SubProcessNative : public ObjectBackedNativeModule {
 public:
  SubProcessNative(ScriptContext* context);
  ~SubProcessNative() override;

  void ProcessOpen(const v8::FunctionCallbackInfo<v8::Value>& info);
};

}  // namespace zarun

#endif  // ZARUN_MODULES_SUBPROCESS_H_
