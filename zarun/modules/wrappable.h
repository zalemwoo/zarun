/*
 * wrappable.h
 *
 */

// TODO(zalemwoo) Because seems that gin::ObjectTemplateBuilder::SetMethod
// cause memory leak. currently use this.
// See: https://code.google.com/p/chromium/issues/detail?id=431213
// on 2014/12/03

#ifndef ZARUN_MODULES_WRAPPABLE_H_
#define ZARUN_MODULES_WRAPPABLE_H_

#include "base/macros.h"
#include "base/template_util.h"
#include "gin/public/gin_embedders.h"
#include "gin/converter.h"
#include "v8/include/v8.h"

#include "zarun/zarun_export.h"

namespace zarun {

enum Embedder { kZarunEmbedder = gin::GinEmbedder::kEmbedderBlink + 1 };

enum InternalFields {
  kWrapperInfoIndex,
  kEncodedValueIndex,
  kNumberOfInternalFields,
};

struct ZARUN_EXPORT WrapperInfo {
  static WrapperInfo* From(v8::Handle<v8::Object> object);
  const Embedder embedder;
};

namespace internal {

ZARUN_EXPORT void* FromV8Impl(v8::Isolate* isolate,
                              v8::Handle<v8::Value> val,
                              WrapperInfo* info);

}  // namespace internal

// Wrappable is a base class for C++ objects that have corresponding v8 wrapper
// objects. To retain a Wrappable object on the stack, use a gin::Handle.
//
// USAGE:
// // my_class.h
// class MyClass : Wrappable<MyClass> {
//  public:
//   static WrapperInfo kWrapperInfo;
//
//   // Optional, only required if non-empty template should be used.
//   virtual v8::Handle<v8::ObjectTemplate> GetObjectTemplate(
//       v8::Isolate* isolate);
//   ...
// };
//
// // my_class.cc
// WrapperInfo MyClass::kWrapperInfo = {kZarunEmbedder};
//
// v8::Handle<v8::ObjectTemplate> MyClass::GetObjectTemplate(
//     v8::Isolate* isolate) {
//   return Wrappable<MyClass>::GetObjectTemplate(isolate);
// }
//
// Subclasses should also typically have private constructors and expose a
// static Create function that returns a gin::Handle. Forcing creators through
// this static Create function will enforce that clients actually create a
// wrapper for the object. If clients fail to create a wrapper for a wrappable
// object, the object will leak because we use the weak callback from the
// wrapper as the signal to delete the wrapped object.
template <typename T>
class Wrappable;

// Non-template base class to share code between templates instances.
class ZARUN_EXPORT WrappableBase {
 protected:
  WrappableBase();
  virtual ~WrappableBase();

  virtual v8::Handle<v8::ObjectTemplate> GetObjectTemplate(
      v8::Isolate* isolate);

  v8::Handle<v8::Object> GetWrapperImpl(v8::Isolate* isolate,
                                        WrapperInfo* wrapper_info);

 private:
  static void WeakCallback(
      const v8::WeakCallbackData<v8::Object, WrappableBase>& data);

  v8::Persistent<v8::Object> wrapper_;  // Weak

  DISALLOW_COPY_AND_ASSIGN(WrappableBase);
};

template <typename T>
class Wrappable : public WrappableBase {
 public:
  // Retrieve (or create) the v8 wrapper object cooresponding to this object.
  // To customize the wrapper created for a subclass, override GetWrapperInfo()
  // instead of overriding this function.
  v8::Handle<v8::Object> GetWrapper(v8::Isolate* isolate) {
    return GetWrapperImpl(isolate, &T::kWrapperInfo);
  }

 protected:
  Wrappable() {}
  virtual ~Wrappable() {}

 private:
  DISALLOW_COPY_AND_ASSIGN(Wrappable);
};

}  // namespace zarun

namespace gin {

// This converter handles any subclass of Wrappable.
template <typename T>
struct Converter<
    T*,
    typename base::enable_if<
        base::is_convertible<T*, zarun::WrappableBase*>::value>::type> {
  static v8::Handle<v8::Value> ToV8(v8::Isolate* isolate, T* val) {
    return val->GetWrapper(isolate);
  }

  static bool FromV8(v8::Isolate* isolate, v8::Handle<v8::Value> val, T** out) {
    *out = static_cast<T*>(static_cast<zarun::WrappableBase*>(
        zarun::internal::FromV8Impl(isolate, val, &T::kWrapperInfo)));
    return *out != NULL;
  }
};
}

#endif  // ZARUN_MODULES_WRAPPABLE_H_
