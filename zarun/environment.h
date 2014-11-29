/*
 * environment.h
 *
 */

#ifndef ZARUN_ENVIRONMENT_H_
#define ZARUN_ENVIRONMENT_H_

#include "base/files/file_path.h"

#include "gin/public/isolate_holder.h"
#include "v8/include/v8.h"

#include "zarun/zarun_export.h"
#include "zarun/script_context.h"
#include "zarun/modules/javascript_module_system.h"

namespace gin {
class ContextHolder;
}

namespace zarun {

class ZARUN_EXPORT Environment {
 public:
  class StringSourceMap;

  Environment(v8::Isolate* isolate,
              zarun::ScriptContextDelegate* script_context_delegate);

  ~Environment();

  // get Environment from context, maybe NULL
  static Environment* From(v8::Handle<v8::Context> context);

  // Register a named JS module in the module system.
  void RegisterModule(const std::string& name, const std::string& code);

  void RegisterModuleFileForTest(const std::string& name,
                                 const base::FilePath& relate_path);

  JavaScriptModuleSystem* module_system() {
    return script_context_->module_system();
  }

  zarun::ScriptContext* context() { return script_context_.get(); }
  v8::Isolate* isolate();
  v8::Local<v8::Context> v8_context();

 private:
  v8::Isolate* isolate_;
  scoped_ptr<gin::ContextHolder> context_holder_;
  v8::HandleScope handle_scope_;
  scoped_ptr<zarun::ScriptContext> script_context_;
  scoped_ptr<StringSourceMap> source_map_;
};

}  // namespace zarun

#endif  // ZARUN_ENVIRONMENT_H_
