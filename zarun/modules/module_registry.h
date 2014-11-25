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
#include "base/macros.h"
#include "base/memory/scoped_ptr.h"
#include "v8/include/v8.h"

#include "zarun/zarun_export.h"
#include "zarun/modules/builtin_module_provider.h"

namespace zarun {

class ZARUN_EXPORT ModuleRegistry {
 public:
  typedef base::Callback<void(v8::Handle<v8::Value>)> LoadModuleCallback;

  virtual ~ModuleRegistry();

  static ModuleRegistry* From(v8::Handle<v8::Context> context);

  void SetBuiltinModuleProvider(BuiltinModuleProvider* builtin_module_provider);

  void RegisterBuiltinModule(const std::string& id, ModuleGetter getter);

  // The caller must have already entered our context.
  void LoadModule(v8::Isolate* isolate, const std::string& id,
                  LoadModuleCallback callback);

  bool HasModule(v8::Isolate* isolate, const std::string& id,
                 v8::Handle<v8::Value>& out);

  const std::set<std::string>& available_modules() const {
    return available_modules_;
  }

 private:
  explicit ModuleRegistry(v8::Isolate* isolate);

  v8::Handle<v8::Value> GetModule(v8::Isolate* isolate, const std::string& id);

  scoped_ptr<BuiltinModuleProvider> builtin_module_provider_;
  std::set<std::string> available_modules_;
  v8::Persistent<v8::Object> modules_;

  DISALLOW_COPY_AND_ASSIGN(ModuleRegistry);
};

}  // namespace zarun

#endif /* ZARUN_MODULES_MODULE_REGISTRY_H_ */
