/*
 * backend_context_delegate.h
 *
 */

#ifndef ZARUN_BACKEND_BACKEND_CONTEXT_DELEGATE_H_
#define ZARUN_BACKEND_BACKEND_CONTEXT_DELEGATE_H_

#include "base/callback.h"
#include "base/memory/scoped_ptr.h"
#include "gin/try_catch.h"

#include "zarun/zarun_export.h"
#include "zarun/script_context.h"

namespace zarun {
namespace backend {

typedef base::Callback<void(std::string)> RunScriptCallback;

class ZARUN_EXPORT BackendScriptContextDelegate : public ScriptContextDelegate {
 public:
  BackendScriptContextDelegate();
  BackendScriptContextDelegate(RunScriptCallback runscript_callback);
  ~BackendScriptContextDelegate() override;

  void UnhandledException(zarun::ScriptContext* context,
                          gin::TryCatch& try_catch) override;

  void DidCreateEnvironment(ScriptContext* context) override;

 protected:
  // From ShellRunnerDelegate:
  v8::Handle<v8::ObjectTemplate> GetGlobalTemplate(
      v8::Isolate* isolate) override;
  void DidCreateContext(zarun::ScriptContext* context) override;
  void DidRunScript(zarun::ScriptContext* context) override;

 private:
  RunScriptCallback runscript_callback_;
  DISALLOW_COPY_AND_ASSIGN(BackendScriptContextDelegate);
};

scoped_ptr<BackendScriptContextDelegate> CreateBackendScriptContextDelegate();
scoped_ptr<BackendScriptContextDelegate> CreateBackendScriptContextDelegate(
    const RunScriptCallback& runscript_callback);
}
}  // namespace zarun::backend

#endif  // ZARUN_BACKEND_BACKEND_CONTEXT_DELEGATE_H_
