/*
 * wrappable.cc
 *
 */

#include "zarun/modules/wrappable.h"

#include "base/logging.h"

#include "zarun/modules/per_isolate_wrappable_data.h"

namespace zarun {

WrapperInfo* WrapperInfo::From(v8::Handle<v8::Object> object) {
  if (object->InternalFieldCount() != kNumberOfInternalFields)
    return nullptr;
  WrapperInfo* info = static_cast<WrapperInfo*>(
      object->GetAlignedPointerFromInternalField(kWrapperInfoIndex));
  return info->embedder == kZarunEmbedder ? info : nullptr;
}

WrappableBase::WrappableBase() {
}

WrappableBase::~WrappableBase() {
  wrapper_.Reset();
}

v8::Handle<v8::ObjectTemplate> WrappableBase::GetObjectTemplate(
    v8::Isolate* isolate) {
  v8::Handle<v8::ObjectTemplate> object_templ = v8::ObjectTemplate::New();
  object_templ->SetInternalFieldCount(kNumberOfInternalFields);
  return object_templ;
}

void WrappableBase::WeakCallback(
    const v8::WeakCallbackData<v8::Object, WrappableBase>& data) {
  WrappableBase* wrappable = data.GetParameter();
  wrappable->wrapper_.Reset();
  delete wrappable;
}

v8::Handle<v8::Object> WrappableBase::GetWrapperImpl(v8::Isolate* isolate,
                                                     WrapperInfo* info) {
  if (!wrapper_.IsEmpty()) {
    return v8::Local<v8::Object>::New(isolate, wrapper_);
  }

  zarun::PerIsolateWrappableData* data =
      zarun::PerIsolateWrappableData::From(isolate);
  v8::Local<v8::ObjectTemplate> templ = data->GetObjectTemplate(info);
  if (templ.IsEmpty()) {
    templ = GetObjectTemplate(isolate);
    CHECK(!templ.IsEmpty());
    data->SetObjectTemplate(info, templ);
  }
  CHECK_EQ(kNumberOfInternalFields, templ->InternalFieldCount());
  v8::Handle<v8::Object> wrapper = templ->NewInstance();
  // |wrapper| may be empty in some extreme cases, e.g., when
  // Object.prototype.constructor is overwritten.
  if (wrapper.IsEmpty()) {
    // The current wrappable object will be no longer managed by V8. Delete this
    // now.
    delete this;
    return wrapper;
  }
  wrapper->SetAlignedPointerInInternalField(kWrapperInfoIndex, info);
  wrapper->SetAlignedPointerInInternalField(kEncodedValueIndex, this);
  wrapper_.Reset(isolate, wrapper);
  wrapper_.SetWeak(this, WeakCallback);
  return wrapper;
}

namespace internal {

void* FromV8Impl(v8::Isolate* isolate,
                 v8::Handle<v8::Value> val,
                 WrapperInfo* wrapper_info) {
  if (!val->IsObject())
    return NULL;
  v8::Handle<v8::Object> obj = v8::Handle<v8::Object>::Cast(val);
  WrapperInfo* info = WrapperInfo::From(obj);

  // If this fails, the object is not managed by Gin. It is either a normal JS
  // object that's not wrapping any external C++ object, or it is wrapping some
  // C++ object, but that object isn't managed by Gin (maybe Blink).
  if (!info)
    return NULL;

  // If this fails, the object is managed by Gin, but it's not wrapping an
  // instance of the C++ class associated with wrapper_info.
  if (info != wrapper_info)
    return NULL;

  return obj->GetAlignedPointerFromInternalField(kEncodedValueIndex);
}

}  // namespace internal

}  // namespace zarun
