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
public:
  static void ProcessOpenCallback(gin::Arguments* args);

 protected:
  SubProcessNative(ScriptContext* context);
  ~SubProcessNative() override;
  void Invalidate() override;

  gin::ObjectTemplateBuilder GetObjectTemplateBuilder(
      v8::Isolate* isolate) override;

  DECLARE_THIN_MODULE_END(SubProcessNative)

}  // namespace zarun

#endif  // ZARUN_MODULES_SUBPROCESS_H_
