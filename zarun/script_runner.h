/*
 * scrpt_runner.h
 *
 *  Created on: Nov 21, 2014
 *      Author: zalem
 */

#ifndef ZARUN_SCRIPT_RUNNER_H_
#define ZARUN_SCRIPT_RUNNER_H_

#include "gin/runner.h"

#include "zarun/zarun_export.h"

namespace gin {

class ContextHolder;
class TryCatch;
}

namespace zarun {

class ScriptRunner;

// Subclass ScriptRunnerDelegate to customize the behavior of ScriptRunner.
// Typical embedders will want to subclass one of the specialized
// ScriptRunnerDelegates.
class ZARUN_EXPORT ScriptRunnerDelegate {
 public:
  ScriptRunnerDelegate();
  virtual ~ScriptRunnerDelegate();

  // Returns the template for the global object.
  virtual v8::Handle<v8::ObjectTemplate> GetGlobalTemplate(
      ScriptRunner* runner, v8::Isolate* isolate);
  virtual void DidCreateContext(ScriptRunner* runner);
  virtual void WillRunScript(ScriptRunner* runner);
  virtual void DidRunScript(ScriptRunner* runner);
  virtual void UnhandledException(ScriptRunner* runner,
                                  gin::TryCatch& try_catch);

  virtual void ProcessResult(ScriptRunner* runner, v8::Local<v8::Value> result);
};

// ScriptRunner executes the script/functions directly in a v8::Context.
// ScriptRunner owns a ContextHolder and v8::Context, both of which are
// destroyed
// when the ScriptRunner is deleted.
class ZARUN_EXPORT ScriptRunner : public gin::Runner {
 public:
  ScriptRunner(ScriptRunnerDelegate* delegate, v8::Isolate* isolate);
  ~ScriptRunner() override;

  // Before running script in this context, you'll need to enter the runner's
  // context by creating an instance of Runner::Scope on the stack.

  // Runner overrides:
  void Run(const std::string& source,
           const std::string& resource_name) override;
  v8::Handle<v8::Value> Call(v8::Handle<v8::Function> function,
                             v8::Handle<v8::Value> receiver, int argc,
                             v8::Handle<v8::Value> argv[]) override;
  gin::ContextHolder* GetContextHolder() override;

 private:
  friend class gin::Runner::Scope;

  void Run(v8::Handle<v8::Script> script);

  ScriptRunnerDelegate* delegate_;
  scoped_ptr<gin::ContextHolder> context_holder_;

  DISALLOW_COPY_AND_ASSIGN(ScriptRunner);
};

}  // namespace zarun

#endif /* ZARUN_SCRIPT_RUNNER_H_ */
