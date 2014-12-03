/*
 * process_handle_wrapper.h
 *
 */

#ifndef ZARUN_MODULES_CPP_INTERNAL_PROCESS_HANDLE_WRAPPER_H_
#define ZARUN_MODULES_CPP_INTERNAL_PROCESS_HANDLE_WRAPPER_H_

#include "base/process/process_handle.h"
#include "gin/handle.h"

#include "v8/include/v8.h"

#include "zarun/modules/wrappable.h"

namespace zarun {
namespace internal {

class ProcessHandleWrapper : public zarun::Wrappable<ProcessHandleWrapper> {
 public:
  static zarun::WrapperInfo kWrapperInfo;

  static gin::Handle<ProcessHandleWrapper> Create(
      v8::Isolate* isolate,
      base::ProcessHandle process_handle) {
    return gin::CreateHandle(isolate, new ProcessHandleWrapper(process_handle));
  }

  base::ProcessHandle value() const { return process_handle_; }
  void Close();

 protected:
  ProcessHandleWrapper(base::ProcessHandle process_handle);
  ~ProcessHandleWrapper() override;
  v8::Handle<v8::ObjectTemplate> GetObjectTemplate(
      v8::Isolate* isolate) override;

 private:
  base::ProcessHandle process_handle_;
  bool closed_;
};
}
}  // namespace zarun::internal

#endif  // ZARUN_MODULES_CPP_INTERNAL_PROCESS_HANDLE_WRAPPER_H_
