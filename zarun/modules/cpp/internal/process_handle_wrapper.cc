/*
 * process_handle_wrapper.cc
 *
 */

#include "zarun/modules/cpp/internal/process_handle_wrapper.h"

#include "base/logging.h"
#include "base/bind.h"
#include "base/process/process.h"
#include "gin/public/gin_embedders.h"

namespace zarun {
namespace internal {

zarun::WrapperInfo ProcessHandleWrapper::kWrapperInfo = {zarun::kZarunEmbedder};

ProcessHandleWrapper::ProcessHandleWrapper(base::ProcessHandle process_handle)
    : process_handle_(process_handle), closed_(false) {
}

ProcessHandleWrapper::~ProcessHandleWrapper() {
  Close();
}

void ProcessHandleWrapper::Close() {
  if (closed_)
    return;
  closed_ = true;
  base::CloseProcessHandle(process_handle_);
}

v8::Handle<v8::ObjectTemplate> ProcessHandleWrapper::GetObjectTemplate(
    v8::Isolate* isolate) {
  return zarun::Wrappable<ProcessHandleWrapper>::GetObjectTemplate(isolate);
}
}
}  // namespace zarun::internal
