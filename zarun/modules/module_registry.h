/*
 * module_registry.h
 *
 *  Created on: Nov 20, 2014
 *      Author: zalem
 */

#ifndef ZARUN_MODULES_MODULE_REGISTRY_H_
#define ZARUN_MODULES_MODULE_REGISTRY_H_

#include <map>
#include <set>
#include <string>

#include "base/callback.h"
#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/scoped_vector.h"
#include "v8/include/v8.h"

#include "zarun/zarun_export.h"

namespace zarun {

class ZARUN_EXPORT ModuleRegistry {
 public:
  typedef base::Callback<void(v8::Handle<v8::Value>)> LoadModuleCallback;

  virtual ~ModuleRegistry();

  static ModuleRegistry* From(v8::Handle<v8::Context> context);

  // The caller must have already entered our context.
  void AddBuiltinModule(v8::Isolate* isolate, const std::string& id,
                        v8::Handle<v8::Value> module);

  void LoadModule(v8::Isolate* isolate, const std::string& id,
                  LoadModuleCallback callback);

  bool HasModule(v8::Isolate* isolate, const std::string& id,
                 v8::Handle<v8::Value>& out);

  const std::set<std::string>& available_modules() const {
    return available_modules_;
  }

 private:
  typedef std::multimap<std::string, LoadModuleCallback> LoadModuleCallbackMap;

  explicit ModuleRegistry(v8::Isolate* isolate);

  void RegisterModule(v8::Isolate* isolate, const std::string& id,
                      v8::Handle<v8::Value> module);

  v8::Handle<v8::Value> GetModule(v8::Isolate* isolate, const std::string& id);

  std::set<std::string> available_modules_;
  std::set<std::string> unsatisfied_dependencies_;

  LoadModuleCallbackMap waiting_callbacks_;

  v8::Persistent<v8::Object> modules_;

  DISALLOW_COPY_AND_ASSIGN(ModuleRegistry);
};

}  // namespace zarun

#endif /* ZARUN_MODULES_MODULE_REGISTRY_H_ */
