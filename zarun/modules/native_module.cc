#include "zarun/modules/native_module.h"

namespace zarun {

NativeObject::NativeObject(ScriptContext* context) {
  context_ = context;
}

v8::Isolate* NativeObject::isolate() const {
  return context_->isolate();
}

v8::Handle<v8::Context> NativeObject::v8_context() const {
  return context_->v8_context();
}

void NativeModule::Invalidate() {
  this->is_valid_ = false;
}

}  // namespace zarun
