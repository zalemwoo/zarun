/*
 * scrpt_CONTEXT.h
 *
 */

#ifndef ZARUN_SCRIPT_CONTEXT_H_
#define ZARUN_SCRIPT_CONTEXT_H_

#include "gin/runner.h"
#include "zarun/zarun_export.h"

namespace gin {

class ContextHolder;
class TryCatch;
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
      ScriptContext* runner,
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
  ScriptContext(ScriptContextDelegate* delegate, v8::Isolate* isolate);
  ~ScriptContext() override;

  // Before running script in this context, you'll need to enter the runner's
  // context by creating an instance of Runner::Scope on the stack.

  // Runner overrides:
  void Run(const std::string& source,
           const std::string& resource_name) override;
  v8::Handle<v8::Value> Call(v8::Handle<v8::Function> function,
                             v8::Handle<v8::Value> receiver,
                             int argc,
                             v8::Handle<v8::Value> argv[]) override;
  gin::ContextHolder* GetContextHolder() override;

  // Clears this context and invalidates the associated ModuleSystem.
  void Invalidate();
  // Returns true if this context is still valid, false if it isn't.
  // A context becomes invalid via Invalidate().
  bool is_valid() const;

  v8::Handle<v8::Context> v8_context() const;

  static const std::string kReplResultVariableName;

 private:
  friend class gin::Runner::Scope;

  void Run(v8::Handle<v8::Script> script);

  ScriptContextDelegate* delegate_;
  scoped_ptr<gin::ContextHolder> context_holder_;

  DISALLOW_COPY_AND_ASSIGN(ScriptContext);
};

}  // namespace zarun

#endif  // ZARUN_SCRIPT_CONTEXT_H_
