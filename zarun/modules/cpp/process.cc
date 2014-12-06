/*
 * process.cc
 *
 */

#include "zarun/modules/cpp/process.h"

namespace zarun {

void ProcessNative::CloseCallback() {
  process_->Close();
}

void ProcessNative::IsValidCallback(gin::Arguments* args) {
  args->Return(process_->IsValid());
}

DEFINE_WRAPPER_INFO(ProcessNative);

ProcessNative::ProcessNative(ScriptContext* context,
                             base::ProcessHandle process_handle)
    : WrappableNativeObject<ProcessNative>(context),
      process_(new base::Process(process_handle)) {
}

ProcessNative::~ProcessNative() {
  if (process_->IsValid())
    process_->Close();
  process_.reset();
}

gin::ObjectTemplateBuilder ProcessNative::GetObjectTemplateBuilder(
    v8::Isolate* isolate) {
  gin::ObjectTemplateBuilder builder =
      WrappableNativeObject<ProcessNative>::GetObjectTemplateBuilder(isolate);

  builder.SetMethod("close", &ProcessNative::CloseCallback)
      .SetProperty("is_valid", &ProcessNative::IsValidCallback);

  return builder;
}

}  // namespace zarun
