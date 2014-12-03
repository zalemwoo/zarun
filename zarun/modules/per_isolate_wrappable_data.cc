/*
 * per_isolate_wrappable_data.cc
 *
 */

#include "zarun/modules/per_isolate_wrappable_data.h"
#include "zarun/modules/wrappable.h"

namespace zarun {

PerIsolateWrappableData::PerIsolateWrappableData(v8::Isolate* isolate)
    : isolate_(isolate) {
  isolate_->SetData(kZarunEmbedder, this);
}

PerIsolateWrappableData::~PerIsolateWrappableData() {
  isolate_->SetData(kZarunEmbedder, NULL);
}

// static
PerIsolateWrappableData* PerIsolateWrappableData::From(v8::Isolate* isolate) {
  return static_cast<PerIsolateWrappableData*>(
      isolate->GetData(kZarunEmbedder));
}

void PerIsolateWrappableData::SetObjectTemplate(
    WrapperInfo* info,
    v8::Local<v8::ObjectTemplate> templ) {
  object_templates_[info] = v8::Eternal<v8::ObjectTemplate>(isolate_, templ);
}

void PerIsolateWrappableData::SetFunctionTemplate(
    WrapperInfo* info,
    v8::Local<v8::FunctionTemplate> templ) {
  function_templates_[info] =
      v8::Eternal<v8::FunctionTemplate>(isolate_, templ);
}

v8::Local<v8::ObjectTemplate> PerIsolateWrappableData::GetObjectTemplate(
    WrapperInfo* info) {
  ObjectTemplateMap::iterator it = object_templates_.find(info);
  if (it == object_templates_.end())
    return v8::Local<v8::ObjectTemplate>();
  return it->second.Get(isolate_);
}

v8::Local<v8::FunctionTemplate> PerIsolateWrappableData::GetFunctionTemplate(
    WrapperInfo* info) {
  FunctionTemplateMap::iterator it = function_templates_.find(info);
  if (it == function_templates_.end())
    return v8::Local<v8::FunctionTemplate>();
  return it->second.Get(isolate_);
}

}  // namespace zarun
