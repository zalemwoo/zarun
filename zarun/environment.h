/*
 * environment.h
 *
 */

#ifndef ZARUN_ENVIRONMENT_H_
#define ZARUN_ENVIRONMENT_H_

#include "base/files/file_path.h"
#include "base/callback.h"

#include "gin/public/isolate_holder.h"
#include "v8/include/v8.h"

#include "zarun/zarun_export.h"
#include "zarun/script_context.h"

namespace gin {
class ContextHolder;
}

namespace zarun {

// TODO(zalemwoo) : SourceMap is temporary used,  ModuleProvider is preferred.
class NativeSourceMap;
class CommonModuleSystem;

// Environment owns a ContextHolder and v8::Context, both of which are
// destroyed when the Environment is deleted.
class ZARUN_EXPORT Environment {
 public:
  typedef base::Callback<void(Environment*)> EnvironmentCreatedCallback;

  Environment(v8::Isolate* isolate,
              zarun::ScriptContextDelegate* script_context_delegate,
              const EnvironmentCreatedCallback& created_callback);

  ~Environment();

  // get Environment from context, maybe NULL
  static Environment* From(v8::Handle<v8::Context> context);

  // Register a named JS module in the module system.
  void RegisterModule(const std::string& name, const std::string& code);
  void RegisterModuleFileForTest(const std::string& name,
                                 const base::FilePath& relate_path);

  CommonModuleSystem* module_system() {
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
  scoped_ptr<NativeSourceMap> source_map_;

  DISALLOW_COPY_AND_ASSIGN(Environment);
};

}  // namespace zarun

#endif  // ZARUN_ENVIRONMENT_H_
