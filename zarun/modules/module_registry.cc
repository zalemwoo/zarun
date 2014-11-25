/*
 * module_registry.cc
 *
 *  Created on: Nov 20, 2014
 *      Author: zalem
 */

#include "zarun/modules/module_registry.h"

#include <vector>

#include "base/logging.h"
#include "base/macros.h"
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

void ModuleRegistry::SetBuiltinModuleProvider(
    BuiltinModuleProvider* builtin_module_provider) {
  builtin_module_provider_.reset(builtin_module_provider);
}

void ModuleRegistry::RegisterBuiltinModule(const std::string& id,
                                           ModuleGetter getter) {
  DCHECK(builtin_module_provider_);
  builtin_module_provider_->RegisterModule(id, getter);
}

void ModuleRegistry::LoadModule(Isolate* isolate, const std::string& id,
                                LoadModuleCallback callback) {
  v8::HandleScope scope(isolate);
  if (available_modules_.find(id) != available_modules_.end()) {
    // Should we call the callback asynchronously?
    callback.Run(GetModule(isolate, id));
    return;
  }
  v8::Local<v8::Value> module =
      builtin_module_provider_->GetModule(isolate, id);
  if (!module.IsEmpty()) {
    available_modules_.insert(id);
    v8::Local<Object> modules = Local<Object>::New(isolate, modules_);
    modules->Set(gin::StringToSymbol(isolate, id), module);
    callback.Run(module);
    return;
  }

  callback.Run(v8::Local<v8::Value>());
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

v8::Handle<v8::Value> ModuleRegistry::GetModule(v8::Isolate* isolate,
                                                const std::string& id) {
  v8::Handle<Object> modules = Local<Object>::New(isolate, modules_);
  v8::Handle<String> key = gin::StringToSymbol(isolate, id);
  DCHECK(modules->HasOwnProperty(key));
  return modules->Get(key);
}

}  // namespace zarun
