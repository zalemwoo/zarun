/*
 * process.h
 *
 */

#ifndef ZARUN_MODULES_PROCESS_H_
#define ZARUN_MODULES_PROCESS_H_

#include "v8/include/v8.h"

#include "zarun/modules/object_backed_native_module.h"

namespace zarun {

class ScriptContext;

class ProcessNative : public ObjectBackedNativeModule {
 public:
  ProcessNative(ScriptContext* context);
  ~ProcessNative() override;
  v8::Handle<v8::Object> NewInstance() override;
};

}  // namespace zarun

#endif  // ZARUN_MODULES_PROCESS_H_
