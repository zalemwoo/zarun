/*
 * native_object.h
 *
 */

#ifndef ZARUN_MODULES_NATIVE_OBJECT_H_
#define ZARUN_MODULES_NATIVE_OBJECT_H_

#include "gin/public/wrapper_info.h"
#include "gin/handle.h"
#include "gin/wrappable.h"
#include "gin/per_isolate_data.h"
#include "gin/object_template_builder.h"
#include "v8/include/v8.h"

#include "zarun/script_context.h"
#include "zarun/modules/module_macros.h"

namespace zarun {

typedef gin::WrapperInfo WrapperInfo;

class ScriptContext;

class NativeObject {
 public:
  v8::Isolate* isolate() const;
  v8::Handle<v8::Context> v8_context() const;
  ScriptContext* context() const { return context_; }

  virtual v8::Handle<v8::Object> NewInstance() = 0;

 protected:
  NativeObject(ScriptContext* context);
  virtual ~NativeObject() {}

 private:
  ScriptContext* context_;

  DISALLOW_COPY_AND_ASSIGN(NativeObject);
};

// Customize point
// gin::ObjectTemplateBuilder GetObjectTemplateBuilder(v8::Isolate* isolate)
template <typename T>
class WrappableNativeObject : public NativeObject, public gin::Wrappable<T> {
 public:
  template <typename... Args>
  static gin::Handle<T> Create(ScriptContext* context, Args... args) {
    return gin::CreateHandle(context->isolate(), new T(context, args...));
  }

 protected:
  virtual v8::Handle<v8::Object> NewInstance() override {
    return this->GetWrapper(this->isolate());
  }

  WrappableNativeObject(ScriptContext* context)
      : NativeObject(context), gin::Wrappable<T>() {}
  virtual ~WrappableNativeObject() override {}
};

}  // namespace zarun

#endif  // ZARUN_MODULES_NATIVE_OBJECT_H_
