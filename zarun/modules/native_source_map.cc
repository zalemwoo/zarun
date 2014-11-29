/*
 * native_source_map.cc
 *
 */

#include "zarun/modules/native_source_map.h"
#include "base/logging.h"

#define ZARUN_NATIVES_IMPL_
#include "zarun_natives.h"
#undef ZARUN_NATIVES_IMPL_

namespace zarun {

NativeSourceMap::NativeSourceMap() {
  const _zarun_native* natives_ptr = natives;
  while (natives_ptr->name) {
    CHECK(natives_ptr->source && natives_ptr->source_len);
    RegisterModule(natives_ptr->name,
                   std::string(natives_ptr->source, natives_ptr->source_len));
    natives_ptr++;
  }
}

NativeSourceMap::~NativeSourceMap() {
}

v8::Handle<v8::Value> NativeSourceMap::GetSource(v8::Isolate* isolate,
                                                 const std::string& name) {
  if (source_map_.count(name) == 0)
    return v8::Undefined(isolate);
  return v8::String::NewFromUtf8(isolate, source_map_[name].c_str());
}

bool NativeSourceMap::Contains(const std::string& name) {
  return source_map_.count(name);
}

void NativeSourceMap::RegisterModule(const std::string& name,
                                     const std::string& source) {
  CHECK_EQ(0u, source_map_.count(name)) << "Module " << name << " not found";
  source_map_[name] = source;
}
}
// namespace zarun
