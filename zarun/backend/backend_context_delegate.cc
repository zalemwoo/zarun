/*
 * backend_context_delegate.cc
 *
 */

#include "zarun/backend/backend_context_delegate.h"

#include "base/logging.h"
#include "base/command_line.h"
#include "base/bind.h"
#include "base/files/file_util.h"
#include "gin/object_template_builder.h"
#include "gin/converter.h"
#include "v8/include/v8.h"

#include "zarun/zarun_shell.h"
#include "zarun/modules/module_registry.h"
#include "zarun/modules/cpp/process.h"

namespace zarun {
namespace backend {

namespace {
void InstallGlobalModule(zarun::ScriptContext* context,
                         std::string id,
                         v8::Handle<v8::Value> module) {
  v8::Isolate* isolate = context->isolate();
  v8::Handle<v8::Object> globalObj = context->global();
  globalObj->Set(gin::StringToSymbol(isolate, id), module);
}
}  // namespace

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
  v8::Isolate* isolate = context->isolate();
  v8::Handle<v8::Context> v8_context = context->v8_context();
  ModuleRegistry* registry = ModuleRegistry::From(v8_context);
  // register builtin modules
  registry->AddBuiltinModule(isolate, zarun::Process::kModuleName,
                             zarun::Process::GetModule(isolate));

  // setup "process" module into global object
  registry->LoadModule(
      isolate, zarun::Process::kModuleName,
      base::Bind(&InstallGlobalModule, context, zarun::Process::kModuleName));

  registry->AttemptToLoadMoreModules(isolate);
}

void BackendScriptContextDelegate::UnhandledException(
    zarun::ScriptContext* context,
    gin::TryCatch& try_catch) {
  LOG(ERROR) << try_catch.GetStackTrace();
}

void BackendScriptContextDelegate::DidRunScript(zarun::ScriptContext* context) {
  if ((ZarunShell::Mode() == ShellMode::Repl) &&
      (!runscript_callback_.is_null())) {
    v8::Isolate* isolate = context->isolate();
    v8::Local<v8::Value> result = context->global()->GetHiddenValue(
        gin::StringToV8(isolate, ScriptContext::kReplResultVariableName));
    if (result.IsEmpty())
      return;
    v8::String::Utf8Value utf8_value(result);
    std::string str(*utf8_value ? *utf8_value : "<string conversion failed>");
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
