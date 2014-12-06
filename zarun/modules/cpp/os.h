/*
 * os.h
 *
 */

#ifndef ZARUN_MODULES_OS_H_
#define ZARUN_MODULES_OS_H_

#include "zarun/modules/native_module.h"

namespace zarun {

class OSNative : public ThinNativeModule<OSNative> {
 public:
  static const char kModuleName[];
  static WrapperInfo kWrapperInfo;
  using ThinNativeModule<OSNative>::GetModule;

 protected:
  OSNative(ScriptContext* context);
  ~OSNative() override;

  void Invalidate() override;

  gin::ObjectTemplateBuilder GetObjectTemplateBuilder(
      v8::Isolate* isolate) override;
  v8::Handle<v8::Object> NewInstance() override;

 private:
  friend ThinNativeModule<OSNative>;
};

}  // namespace zarun

#endif  // ZARUN_MODULES_OS_H_
