/*
 * process.h
 *
 */

#ifndef ZARUN_MODULES_CPP_PROCESS_H_
#define ZARUN_MODULES_CPP_PROCESS_H_

#include "base/memory/scoped_ptr.h"
#include "base/process/process_handle.h"
#include "base/process/process.h"

#include "zarun/modules/native_object.h"

namespace zarun {

class ProcessNative : public WrappableNativeObject<ProcessNative> {
 public:
  static WrapperInfo kWrapperInfo;

  void CloseCallback();
  void IsValidCallback(gin::Arguments* args);

 protected:
  ProcessNative(ScriptContext* context, base::ProcessHandle process_handle);
  ~ProcessNative() override;

  gin::ObjectTemplateBuilder GetObjectTemplateBuilder(
      v8::Isolate* isolate) override;

 private:
  scoped_ptr<base::Process> process_;

  friend WrappableNativeObject<ProcessNative>;
};

}  // namespace zarun

#endif  // ZARUN_MODULES_CPP_PROCESS_H_
