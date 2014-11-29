/*
 * native_source_map.h
 *
 */

#ifndef ZARUN_MODULES_NATIVE_SOURCE_MAP_H_
#define ZARUN_MODULES_NATIVE_SOURCE_MAP_H_

#include "zarun/modules/javascript_module_system.h"

namespace zarun {

class Environment;
// TODO(zalemwoo) : make it inherit ModuleProvider
class NativeSourceMap : public JavaScriptModuleSystem::SourceMap {
 public:
  virtual ~NativeSourceMap() override;
  virtual v8::Handle<v8::Value> GetSource(v8::Isolate* isolate,
                                          const std::string& name) override;
  virtual bool Contains(const std::string& name) override;

  void RegisterModule(const std::string& name, const std::string& source);

 private:
  std::map<std::string, std::string> source_map_;

  NativeSourceMap();
  friend class zarun::Environment;

  DISALLOW_COPY_AND_ASSIGN(NativeSourceMap);
};

}  // namespace zarun

#endif  // ZARUN_MODULES_NATIVE_SOURCE_MAP_H_
