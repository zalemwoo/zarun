/*
 * module_registry.cc
 *
 *  Created on: Nov 20, 2014
 *      Author: zalem
 */

#include "zarun/modules/module_registry.h"

#include <vector>

#include "base/logging.h"
#include "gin/arguments.h"
#include "gin/converter.h"
#include "gin/per_context_data.h"
#include "gin/per_isolate_data.h"
#include "gin/public/wrapper_info.h"
#include "gin/runner.h"

using v8::Context;
using v8::External;
using v8::Function;
using v8::FunctionTemplate;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::ObjectTemplate;
using v8::Persistent;
using v8::StackTrace;
using v8::String;
using v8::Value;

namespace zarun {

namespace {

// Key for base::SupportsUserData::Data.
const char kModuleRegistryKey[] = "ModuleRegistry";

struct ModuleRegistryData : public base::SupportsUserData::Data {
  scoped_ptr<ModuleRegistry> registry;
};

}  // namespace

ModuleRegistry::ModuleRegistry(Isolate* isolate)
    : modules_(isolate, Object::New(isolate)) {}

ModuleRegistry::~ModuleRegistry() { modules_.Reset(); }

// static
ModuleRegistry* ModuleRegistry::From(v8::Handle<Context> context) {
  gin::PerContextData* data = gin::PerContextData::From(context);
  if (!data) return NULL;

  ModuleRegistryData* registry_data =
      static_cast<ModuleRegistryData*>(data->GetUserData(kModuleRegistryKey));

  if (!registry_data) {
    // PerContextData takes ownership of ModuleRegistryData.
    registry_data = new ModuleRegistryData;
    registry_data->registry.reset(new ModuleRegistry(context->GetIsolate()));
    data->SetUserData(kModuleRegistryKey, registry_data);
  }
  return registry_data->registry.get();
}

void ModuleRegistry::AddBuiltinModule(Isolate* isolate, const std::string& id,
                                      v8::Handle<Value> module) {
  DCHECK(!id.empty());
  RegisterModule(isolate, id, module);
}

void ModuleRegistry::LoadModule(Isolate* isolate, const std::string& id,
                                LoadModuleCallback callback) {
  if (available_modules_.find(id) != available_modules_.end()) {
    // Should we call the callback asynchronously?
    callback.Run(GetModule(isolate, id));
    return;
  }
  waiting_callbacks_.insert(std::make_pair(id, callback));
  unsatisfied_dependencies_.insert(id);
}

bool ModuleRegistry::HasModule(v8::Isolate* isolate, const std::string& id,
                               v8::Handle<v8::Value>& out) {
  v8::Handle<Object> modules = Local<Object>::New(isolate, modules_);
  v8::Handle<String> key = gin::StringToSymbol(isolate, id);
  if (modules->HasOwnProperty(key)) {
    out = modules->Get(key);
    return true;
  }
  return false;
}

void ModuleRegistry::RegisterModule(Isolate* isolate, const std::string& id,
                                    v8::Handle<Value> module) {
  if (id.empty() || module.IsEmpty()) return;

  unsatisfied_dependencies_.erase(id);
  available_modules_.insert(id);

  v8::Handle<Object> modules = Local<Object>::New(isolate, modules_);
  modules->Set(gin::StringToSymbol(isolate, id), module);

  auto range = waiting_callbacks_.equal_range(id);
  std::vector<LoadModuleCallback> callbacks;
  callbacks.reserve(waiting_callbacks_.count(id));
  for (auto it = range.first; it != range.second; ++it) {
    callbacks.push_back(it->second);
  }
  waiting_callbacks_.erase(range.first, range.second);
  for (auto it = callbacks.begin(); it != callbacks.end(); ++it) {
    // Should we call the callback asynchronously?
    it->Run(module);
  }
}

v8::Handle<v8::Value> ModuleRegistry::GetModule(v8::Isolate* isolate,
                                                const std::string& id) {
  v8::Handle<Object> modules = Local<Object>::New(isolate, modules_);
  v8::Handle<String> key = gin::StringToSymbol(isolate, id);
  DCHECK(modules->HasOwnProperty(key));
  return modules->Get(key);
}

}  // namespace zarun
