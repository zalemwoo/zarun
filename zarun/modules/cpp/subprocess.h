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

class ScriptContext;

class SubProcessNative : public ThinNativeModule<SubProcessNative> {
 public:
  static const char kModuleName[];
  static WrapperInfo kWrapperInfo;
  using ThinNativeModule<SubProcessNative>::GetModule;

  static void ProcessOpenCallback(gin::Arguments* args);

 protected:
  SubProcessNative(ScriptContext* context);
  ~SubProcessNative() override;
  void Invalidate() override;

  gin::ObjectTemplateBuilder GetObjectTemplateBuilder(
      v8::Isolate* isolate) override;

 private:
  friend ThinNativeModule<SubProcessNative>;
};

}  // namespace zarun

#endif  // ZARUN_MODULES_SUBPROCESS_H_
