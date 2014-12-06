
#ifndef ZARUN_MODULES_NATIVE_JAVASCRIPT_MODULE_H_
#define ZARUN_MODULES_NATIVE_JAVASCRIPT_MODULE_H_

#include "zarun/modules/native_object.h"

namespace zarun {

// NativeModule are intended to be used with a ModuleSystem.
// The ModuleSystem will assume ownership of the NativeModule,
// and as a ModuleSystem is tied to a single v8::Context, this implies
// that NativeModule will also be tied to a single v8::Context.
class NativeModule {
 public:
  virtual v8::Handle<v8::Object> NewInstance() = 0;
  // Invalidate this object so it cannot be used any more. This is needed
  // because it's possible for this to outlive its owner context. Invalidate
  // must be called before this happens.
  //
  // Subclasses should override to invalidate their own V8 state. If they do
  // they must call their superclass' Invalidate().
  virtual void Invalidate();
  virtual ~NativeModule() {}

 protected:
  NativeModule(bool is_valid = true) : is_valid_(is_valid) {}
  // Allow subclasses to query valid state.
  bool is_valid() { return is_valid_; }

 private:
  bool is_valid_;
};

template <typename T>
class ThinNativeModule : public NativeObject, public NativeModule {
 public:
  static scoped_ptr<ThinNativeModule> GetModule(ScriptContext* context) {
    v8::HandleScope scope(context->isolate());
    gin::PerIsolateData* data = gin::PerIsolateData::From(context->isolate());
    v8::Handle<v8::ObjectTemplate> templ =
        data->GetObjectTemplate(&T::kWrapperInfo);
    DCHECK(templ.IsEmpty());
    scoped_ptr<ThinNativeModule> module(new T(context));
    templ = module->GetObjectTemplateBuilder(context->isolate())
                .Build();  // Customize point.

    data->SetObjectTemplate(&T::kWrapperInfo, templ);
    return module.Pass();
  }

  virtual ~ThinNativeModule() {}

  v8::Handle<v8::Object> NewInstance() override {
    return NativeObject::NewInstance();
  }

 protected:
  ThinNativeModule(ScriptContext* context)
      : NativeObject(context), NativeModule(true) {}

  virtual v8::Handle<v8::ObjectTemplate> GetObjectTemplate() override {
    gin::PerIsolateData* data =
        gin::PerIsolateData::From(NativeObject::isolate());
    return data->GetObjectTemplate(&T::kWrapperInfo);
  }

  virtual void Invalidate() override { NativeModule::Invalidate(); }

  virtual gin::ObjectTemplateBuilder GetObjectTemplateBuilder(
      v8::Isolate* isolate) {
    return gin::ObjectTemplateBuilder(isolate);
  }
};

template <typename T>
class WrappableNativeModule : public WrappableNativeObject<T>,
                              public NativeModule {
 public:
  template <typename... Args>
  static gin::Handle<T> Create(ScriptContext* context, Args... args) {
    return gin::CreateHandle(context->isolate(), new T(context, args...));
  }

  v8::Handle<v8::Object> NewInstance() override {
    return WrappableNativeObject<T>::NewInstance();
  }

 protected:
  WrappableNativeModule(ScriptContext* context)
      : WrappableNativeObject<T>(context), NativeModule(true) {}
  virtual ~WrappableNativeModule() {}
  virtual void Invalidate() override { NativeModule::Invalidate(); }
};

}  // namespace zarun

#endif  // ZARUN_MODULES_NATIVE_JAVASCRIPT_MODULE_H_
