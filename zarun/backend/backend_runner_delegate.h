/*
 * backend_runner_delegate.h
 *
 *  Created on: Nov 19, 2014
 *      Author: zalem
 */

#ifndef ZARUN_BACKEND_BACKEND_RUNNER_DELEGATE_H_
#define ZARUN_BACKEND_BACKEND_RUNNER_DELEGATE_H_

#include <map>

#include "base/callback.h"
#include "gin/try_catch.h"

#include "zarun/zarun_export.h"
#include "zarun/script_runner.h"

namespace zarun {
namespace backend {

typedef v8::Local<v8::Value>(*ModuleGetter)(v8::Isolate* isolate);
typedef base::Callback<v8::Local<v8::Value>(v8::Isolate*)> ModuleGetterCallback;

typedef base::Callback<void(v8::Handle<v8::Value>)> RunScriptCallback;

class ZARUN_EXPORT BackendScriptRunnerDelegate : public ScriptRunnerDelegate {
 public:
  BackendScriptRunnerDelegate();
  BackendScriptRunnerDelegate(RunScriptCallback runscript_callback);
  ~BackendScriptRunnerDelegate() override;
  void UnhandledException(zarun::ScriptRunner* runner,
                          gin::TryCatch& try_catch) override;
  void ProcessResult(ScriptRunner* runner,
                     v8::Local<v8::Value> result) override;

  void AddBuiltinModule(const std::string& id, ModuleGetter getter);
  void AddBuiltinModule(const std::string& id,
                        const ModuleGetterCallback& getter);

 protected:
  // From ShellRunnerDelegate:
  v8::Handle<v8::ObjectTemplate> GetGlobalTemplate(
      zarun::ScriptRunner* runner, v8::Isolate* isolate) override;
  void DidCreateContext(zarun::ScriptRunner* runner) override;
  void DidRunScript(zarun::ScriptRunner* runner) override;

 private:
  typedef std::map<std::string, ModuleGetterCallback> BuiltinModuleMap;
  RunScriptCallback runscript_callback_;
  BuiltinModuleMap builtin_modules_;

  DISALLOW_COPY_AND_ASSIGN(BackendScriptRunnerDelegate);
};
}
}  // namespace zarun::backend

#endif /* ZARUN_BACKEND_BACKEND_RUNNER_DELEGATE_H_ */
