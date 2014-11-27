/*
 * backend_runner_delegate.cc
 *
 */

#include "zarun/backend/backend_runner_delegate.h"

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

void InstallGlobalModule(zarun::ScriptRunner* runner,
                         std::string id,
                         v8::Handle<v8::Value> module) {
  gin::ContextHolder* context_holder = runner->GetContextHolder();
  v8::Isolate* isolate = context_holder->isolate();
  v8::Handle<v8::Object> globalObj = runner->global();
  globalObj->Set(gin::StringToSymbol(isolate, id), module);
}

}  // namespace

BackendScriptRunnerDelegate::BackendScriptRunnerDelegate() {
}

BackendScriptRunnerDelegate::BackendScriptRunnerDelegate(
    RunScriptCallback runscript_callback)
    : BackendScriptRunnerDelegate() {
  runscript_callback_ = runscript_callback;
}

BackendScriptRunnerDelegate::~BackendScriptRunnerDelegate() {
}

v8::Handle<v8::ObjectTemplate> BackendScriptRunnerDelegate::GetGlobalTemplate(
    zarun::ScriptRunner* runner,
    v8::Isolate* isolate) {
  v8::Handle<v8::ObjectTemplate> templ =
      gin::ObjectTemplateBuilder(isolate).Build();
  return templ;
}

void BackendScriptRunnerDelegate::DidCreateContext(
    zarun::ScriptRunner* runner) {
  v8::Handle<v8::Context> context = runner->GetContextHolder()->context();
  ModuleRegistry* registry = ModuleRegistry::From(context);

  registry->SetBuiltinModuleProvider(new BuiltinModuleProvider());

  v8::Isolate* isolate = runner->GetContextHolder()->isolate();

  registry->RegisterBuiltinModule(zarun::Process::kModuleName,
                                  zarun::Process::GetModule);
  registry->RegisterBuiltinModule(zarun::Console::kModuleName,
                                  zarun::Console::GetModule);
  registry->RegisterBuiltinModule(zarun::GC::kModuleName, zarun::GC::GetModule);

  registry->LoadModule(
      isolate, zarun::Process::kModuleName,
      base::Bind(&InstallGlobalModule, runner, zarun::Process::kModuleName));
}

void BackendScriptRunnerDelegate::UnhandledException(
    zarun::ScriptRunner* runner,
    gin::TryCatch& try_catch) {
  LOG(ERROR) << try_catch.GetStackTrace();
}

void BackendScriptRunnerDelegate::DidRunScript(zarun::ScriptRunner* runner) {
  if ((ZarunShell::Mode() == ShellMode::Repl) &&
      (!runscript_callback_.is_null())) {
    v8::Isolate* isolate = runner->GetContextHolder()->isolate();
    v8::Local<v8::Value> result = runner->global()->GetHiddenValue(
        gin::StringToV8(isolate, ScriptRunner::kReplResultVariableName));
    if (result.IsEmpty())
      return;
    v8::String::Utf8Value utf8_value(result);
    std::string str(*utf8_value ? *utf8_value : "<string conversion failed>");
    runscript_callback_.Run(str);
  }
}

scoped_ptr<BackendScriptRunnerDelegate> CreateBackendScriptRunnerDelegate() {
  scoped_ptr<BackendScriptRunnerDelegate> delegate;
  delegate.reset(new BackendScriptRunnerDelegate());
  return delegate.Pass();
}

scoped_ptr<BackendScriptRunnerDelegate> CreateBackendScriptRunnerDelegate(
    const RunScriptCallback& runscript_callback) {
  scoped_ptr<BackendScriptRunnerDelegate> delegate;
  delegate.reset(new BackendScriptRunnerDelegate(runscript_callback));
  return delegate.Pass();
}
}
}  // namespace zarun::backend
