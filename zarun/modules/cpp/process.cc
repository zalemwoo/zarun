/*
 * process.cc
 *
 */

#include "zarun/modules/cpp/process.h"

namespace zarun {

gin::WrapperInfo ProcessNative::kWrapperInfo = {gin::kEmbedderNativeGin};

void ProcessNative::Close() {
  process_->Close();
}

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

  builder.SetMethod("close",
                    base::Bind(&ProcessNative::Close, base::Unretained(this)));

  return builder;
}

}  // namespace zarun
