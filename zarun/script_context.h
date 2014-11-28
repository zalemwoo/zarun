/*
 * scrpt_CONTEXT.h
 *
 */

#ifndef ZARUN_SCRIPT_CONTEXT_H_
#define ZARUN_SCRIPT_CONTEXT_H_

#include "gin/runner.h"
#include "zarun/zarun_export.h"
#include "zarun/modules/javascript_module_system.h"
#include "zarun/safe_builtins.h"
#include "v8/include/v8.h"

namespace gin {
class TryCatch;
class PerContextData;
}

namespace zarun {

class ScriptContext;

// Subclass ScriptContextDelegate to customize the behavior of ScriptContext.
// Typical embedders will want to subclass one of the specialized
// ScriptContextDelegates.
class ZARUN_EXPORT ScriptContextDelegate {
 public:
  ScriptContextDelegate();
  virtual ~ScriptContextDelegate();

  // Returns the template for the global object.
  virtual v8::Handle<v8::ObjectTemplate> GetGlobalTemplate(
      v8::Isolate* isolate);
  virtual void DidCreateContext(ScriptContext* runner);
  virtual void WillRunScript(ScriptContext* runner);
  virtual void DidRunScript(ScriptContext* runner);
  virtual void UnhandledException(ScriptContext* runner,
                                  gin::TryCatch& try_catch);
};

// ScriptContext executes the script/functions directly in a v8::Context.
// ScriptContext owns a ContextHolder and v8::Context, both of which are
// destroyed when the ScriptContext is deleted.
class ZARUN_EXPORT ScriptContext : public gin::Runner {
 public:
  ScriptContext(ScriptContextDelegate* delegate,
                const v8::Handle<v8::Context>& v8_context);
  ~ScriptContext() override;

  // Runs |function| with appropriate scopes. Doesn't catch exceptions, callers
  // must do that if they want.
  //
  // USE THIS METHOD RATHER THAN v8::Function::Call WHEREVER POSSIBLE.
  v8::Local<v8::Value> CallFunction(v8::Handle<v8::Function> function,
                                    int argc,
                                    v8::Handle<v8::Value> argv[]) const;

  void set_module_system(scoped_ptr<JavaScriptModuleSystem> module_system) {
    module_system_ = module_system.Pass();
  }

  JavaScriptModuleSystem* module_system() { return module_system_.get(); }

  SafeBuiltins* safe_builtins() { return &safe_builtins_; }
  const SafeBuiltins* safe_builtins() const { return &safe_builtins_; }

  // Before running script in this context, you'll need to enter the runner's
  // context by creating an instance of Runner::Scope on the stack.
  // gin::Runner overrides:
  void Run(const std::string& source,
           const std::string& resource_name) override;
  v8::Handle<v8::Value> Call(v8::Handle<v8::Function> function,
                             v8::Handle<v8::Value> receiver,
                             int argc,
                             v8::Handle<v8::Value> argv[]) override;

  // Clears this context and invalidates the associated ModuleSystem.
  void Invalidate();
  // Returns true if this context is still valid, false if it isn't.
  // A context becomes invalid via Invalidate().
  bool is_valid() const;

  v8::Handle<v8::Context> v8_context() const;
  v8::Isolate* isolate() const { return isolate_; }

  static const std::string kReplResultVariableName;

  gin::ContextHolder* GetContextHolder() override;

 private:
  friend class gin::Runner::Scope;

  void Run(v8::Handle<v8::Script> script);

  ScriptContextDelegate* delegate_;

  // The v8 context the bindings are accessible to.
  ScopedPersistent<v8::Context> v8_context_;

  // Owns and structures the JS that is injected to set up extension bindings.
  scoped_ptr<JavaScriptModuleSystem> module_system_;

  // Contains safe copies of builtin objects like Function.prototype.
  SafeBuiltins safe_builtins_;

  v8::Isolate* isolate_;

  DISALLOW_COPY_AND_ASSIGN(ScriptContext);
};

}  // namespace zarun

#endif  // ZARUN_SCRIPT_CONTEXT_H_
