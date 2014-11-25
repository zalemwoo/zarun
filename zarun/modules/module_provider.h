/*
 * module_provider.h
 *
 */

#ifndef ZARUN_MODULES_MODULE_PROVIDER_H_
#define ZARUN_MODULES_MODULE_PROVIDER_H_

#include <map>
#include <string>

#include "base/macros.h"
#include "base/bind.h"
#include "v8/include/v8.h"

namespace zarun {

typedef v8::Local<v8::Value>(*ModuleGetter)(v8::Isolate* isolate);

class ModuleRegistry;

class ModuleProvider {
 public:
  typedef base::Callback<v8::Local<v8::Value>(v8::Isolate*)>
      ModuleGetterCallback;

  ModuleProvider();
  virtual ~ModuleProvider();

  void RegisterModule(const std::string& id, ModuleGetter getter);
  void RegisterModule(const std::string& id,
                      const ModuleGetterCallback& getter);

 protected:
  typedef std::map<std::string, ModuleGetterCallback> BuiltinModuleMap;
  BuiltinModuleMap registered_modules_;
  virtual v8::Local<v8::Value> GetModule(v8::Isolate* isolate,
                                         const std::string& id) = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(ModuleProvider);
};

}  // zarun

#endif  // ZARUN_MODULES_MODULE_PROVIDER_H_
