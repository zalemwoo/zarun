/*
 * module_provider.h
 *
 */

#ifndef ZARUN_BUILTIN_MODULES_MODULE_PROVIDER_H_
#define ZARUN_BUILTIN_MODULES_MODULE_PROVIDER_H_

#include "zarun/modules/module_provider.h"

namespace zarun {

class BuiltinModuleProvider : public ModuleProvider {
 public:
  BuiltinModuleProvider();
  virtual ~BuiltinModuleProvider();

 private:
  v8::Local<v8::Value> GetModule(v8::Isolate* isolate,
                                 const std::string& id) override;
  friend class ModuleRegistry;
};

}  // zarun

#endif  // ZARUN_BUILTIN_MODULES_MODULE_PROVIDER_H_
