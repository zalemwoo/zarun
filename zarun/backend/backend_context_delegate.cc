/*
 * backend_context_delegate.cc
 *
 */

#include "zarun/backend/backend_context_delegate.h"

#include "base/logging.h"
#include "base/bind.h"
#include "base/files/file_util.h"
#include "gin/object_template_builder.h"
#include "gin/converter.h"
#include "v8/include/v8.h"

#include "zarun/zarun_shell.h"
#include "zarun/modules/module_registry.h"
#include "zarun/modules/builtin_module_provider.h"
#include "zarun/modules/cpp/console.h"
#include "zarun/modules/cpp/process.h"
#include "zarun/modules/cpp/gc.h"

namespace zarun {
namespace backend {

namespace {

void InstallGlobalModule(zarun::ScriptContext* context,
                         std::string id,
                         v8::Handle<v8::Value> module) {
  gin::ContextHolder* context_holder = context->GetContextHolder();
  v8::Isolate* isolate = context_holder->isolate();
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
    zarun::ScriptContext* context,
    v8::Isolate* isolate) {
  v8::EscapableHandleScope handle_scope(isolate);
  v8::Local<v8::ObjectTemplate> templ =
      gin::ObjectTemplateBuilder(isolate).Build();
  return handle_scope.Escape(templ);
}

void BackendScriptContextDelegate::DidCreateContext(
    zarun::ScriptContext* context) {
  v8::Handle<v8::Context> v8_context = context->v8_context();
  ModuleRegistry* registry = ModuleRegistry::From(v8_context);

  registry->SetBuiltinModuleProvider(new BuiltinModuleProvider());

  v8::Isolate* isolate = context->GetContextHolder()->isolate();

  registry->RegisterBuiltinModule(zarun::Process::kModuleName,
                                  zarun::Process::GetModule);
  registry->RegisterBuiltinModule(zarun::Console::kModuleName,
                                  zarun::Console::GetModule);
  registry->RegisterBuiltinModule(zarun::GC::kModuleName, zarun::GC::GetModule);

  registry->LoadModule(
      isolate, zarun::Process::kModuleName,
      base::Bind(&InstallGlobalModule, context, zarun::Process::kModuleName));
}

void BackendScriptContextDelegate::UnhandledException(
    zarun::ScriptContext* context,
    gin::TryCatch& try_catch) {
  LOG(ERROR) << try_catch.GetStackTrace();
}

void BackendScriptContextDelegate::DidRunScript(zarun::ScriptContext* context) {
  if ((ZarunShell::Mode() == ShellMode::Repl) &&
      (!runscript_callback_.is_null())) {
    v8::Isolate* isolate = context->GetContextHolder()->isolate();
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
