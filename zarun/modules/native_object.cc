/*
 * native_object.cc
 *
 */

#include "zarun/modules/native_object.h"
#include "zarun/script_context.h"

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

v8::Handle<v8::Object> NativeObject::NewInstance() {
  v8::Local<v8::ObjectTemplate> templ = GetObjectTemplate();
  DCHECK(!templ.IsEmpty());
  return templ->NewInstance();
}

}  // namespace zarun
