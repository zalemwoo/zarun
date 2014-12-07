/*
 * process.cc
 *
 */

#include "zarun/modules/cpp/process_wrapper.h"

namespace zarun {

void ProcessWrapper::CloseCallback() {
  process_->Close();
}

void ProcessWrapper::IsValidCallback(gin::Arguments* args) {
  args->Return(process_->IsValid());
}

DEFINE_WRAPPER_INFO(ProcessWrapper);

ProcessWrapper::ProcessWrapper(ScriptContext* context,
                               base::ProcessHandle process_handle)
    : WrappableNativeObject<ProcessWrapper>(context),
      process_(new base::Process(process_handle)) {
}

ProcessWrapper::~ProcessWrapper() {
  if (process_->IsValid())
    process_->Close();
  process_.reset();
}

gin::ObjectTemplateBuilder ProcessWrapper::GetObjectTemplateBuilder(
    v8::Isolate* isolate) {
  gin::ObjectTemplateBuilder builder =
      WrappableNativeObject<ProcessWrapper>::GetObjectTemplateBuilder(isolate);

  builder.SetMethod("close", &ProcessWrapper::CloseCallback)
      .SetProperty("is_valid", &ProcessWrapper::IsValidCallback);

  return builder;
}

}  // namespace zarun
