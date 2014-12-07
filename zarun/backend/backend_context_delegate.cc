/*
 * backend_context_delegate.cc
 *
 */

#include "zarun/backend/backend_context_delegate.h"

#include "base/logging.h"
#include "base/bind.h"
#include "gin/object_template_builder.h"
#include "v8/include/v8.h"

#include "zarun/zarun_shell.h"

namespace zarun {
namespace backend {

BackendScriptContextDelegate::BackendScriptContextDelegate() {
}

BackendScriptContextDelegate::BackendScriptContextDelegate(
    RunScriptCallback runscript_callback)
    : BackendScriptContextDelegate() {
  runscript_callback_ = runscript_callback;
}

BackendScriptContextDelegate::~BackendScriptContextDelegate() {
}

v8::Handle<v8::ObjectTemplate> BackendScriptContextDelegate::GetGlobalTemplate(
    v8::Isolate* isolate) {
  v8::EscapableHandleScope handle_scope(isolate);
  v8::Local<v8::ObjectTemplate> templ =
      gin::ObjectTemplateBuilder(isolate).Build();
  return handle_scope.Escape(templ);
}

void BackendScriptContextDelegate::DidCreateContext(
    zarun::ScriptContext* context) {
}

void BackendScriptContextDelegate::UnhandledException(
    zarun::ScriptContext* context,
    gin::TryCatch& try_catch) {
  LOG(ERROR) << try_catch.GetStackTrace();
}

void BackendScriptContextDelegate::DidRunScript(zarun::ScriptContext* context) {
  if ((ZarunShell::Mode() == ShellMode::Repl) &&
      (!runscript_callback_.is_null())) {
    v8::Local<v8::Value> result =
        context->global()->GetHiddenValue(gin::StringToV8(
            context->isolate(), ScriptContext::kReplResultVariableName));
    if (result.IsEmpty())
      return;
    std::string str(*v8::String::Utf8Value(result));
    runscript_callback_.Run(str);
  }
}

scoped_ptr<BackendScriptContextDelegate> CreateBackendScriptContextDelegate() {
  scoped_ptr<BackendScriptContextDelegate> delegate;
  delegate.reset(new BackendScriptContextDelegate());
  return delegate.Pass();
}

scoped_ptr<BackendScriptContextDelegate> CreateBackendScriptContextDelegate(
    const RunScriptCallback& runscript_callback) {
  scoped_ptr<BackendScriptContextDelegate> delegate;
  delegate.reset(new BackendScriptContextDelegate(runscript_callback));
  return delegate.Pass();
}
}
}  // namespace zarun::backend
