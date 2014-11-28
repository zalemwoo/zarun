/*
 * environment.h
 *
 */

#ifndef ZARUN_ENVIRONMENT_H_
#define ZARUN_ENVIRONMENT_H_

#include "zarun/zarun_export.h"
#include "zarun/script_context.h"

#include "gin/public/isolate_holder.h"
#include "v8/include/v8.h"

namespace gin {
class ContextHolder;
}

namespace zarun {

class ZARUN_EXPORT Environment {
 public:
  class StringSourceMap;

  static Environment* Create(
      zarun::ScriptContextDelegate* script_context_delegate);

  ~Environment();

  // get Environment from context, maybe NULL
  static Environment* From(v8::Handle<v8::Context> context);

  // Register a named JS module in the module system.
  void RegisterModule(const std::string& name, const std::string& code);

  zarun::ScriptContext* context() { return script_context_.get(); }
  v8::Isolate* isolate();
  v8::Local<v8::Context> v8_context();

 private:
  Environment(zarun::ScriptContextDelegate* script_context_delegate);
  gin::IsolateHolder isolate_holder_;
  scoped_ptr<gin::ContextHolder> context_holder_;
  scoped_ptr<zarun::ScriptContext> script_context_;
  scoped_ptr<StringSourceMap> source_map_;
};

}  // namespace zarun

#endif  // ZARUN_ENVIRONMENT_H_
