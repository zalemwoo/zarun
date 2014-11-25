/*
 * module_provider.cc
 *
 *  Created on: Nov 25, 2014
 *      Author: zalem
 */

#include "zarun/modules/builtin_module_provider.h"

#include "base/bind.h"

namespace zarun {

BuiltinModuleProvider::BuiltinModuleProvider() {}

BuiltinModuleProvider::~BuiltinModuleProvider() {}

v8::Local<v8::Value> BuiltinModuleProvider::GetModule(v8::Isolate* isolate,
                                                      const std::string& id) {
  BuiltinModuleMap::const_iterator it = registered_modules_.find(id);
  if (it != registered_modules_.end()) {
    return it->second.Run(isolate);
  }
  return v8::Local<v8::Value>();
}

}  // namespace zarun
