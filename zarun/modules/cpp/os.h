/*
 * os.h
 *
 */

#ifndef ZARUN_MODULES_OS_H_
#define ZARUN_MODULES_OS_H_

#include "zarun/modules/native_module.h"

namespace zarun {

DECLARE_THIN_MODULE(OSNative)
 protected:
  OSNative(ScriptContext* context);
  ~OSNative() override;

  void Invalidate() override;

  gin::ObjectTemplateBuilder GetObjectTemplateBuilder(
      v8::Isolate* isolate) override;
  v8::Handle<v8::Object> NewInstance() override;

  DECLARE_THIN_MODULE_END(OSNative)

}  // namespace zarun

#endif  // ZARUN_MODULES_OS_H_
