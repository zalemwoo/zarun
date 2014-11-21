/*
 * backend_runner_delegate.cc
 *
 *  Created on: Nov 19, 2014
 *      Author: zalem
 */

#include "zarun/backend/backend_runner_delegate.h"

#include "base/logging.h"
#include "base/bind.h"
#include "base/files/file_util.h"
#include "gin/modules/console.h"
#include "gin/object_template_builder.h"
#include "gin/converter.h"
#include "v8/include/v8.h"

#include "zarun/modules/module_registry.h"
#include "zarun/modules/console.h"
#include "zarun/modules/process.h"

namespace zarun {
namespace backend {

namespace {

void InstallGlobalModule(zarun::ScriptRunner* runner, std::string id,
                         v8::Handle<v8::Value> module) {
  gin::ContextHolder* context_holder = runner->GetContextHolder();
  v8::Handle<v8::Context> context = context_holder->context();
  v8::Isolate* isolate = context_holder->isolate();
  v8::Handle<v8::Object> globalObj = context->Global();

  globalObj->Set(gin::StringToSymbol(isolate, id), module);
}
}
// namespace

BackendScriptRunnerDelegate::BackendScriptRunnerDelegate() {
  AddBuiltinModule(zarun::Console::kModuleName, zarun::Console::GetModule);
  AddBuiltinModule(zarun::Process::kModuleName, zarun::Process::GetModule);
}

BackendScriptRunnerDelegate::BackendScriptRunnerDelegate(
    RunScriptCallback runscript_callback)
    : BackendScriptRunnerDelegate() {
  runscript_callback_ = runscript_callback;
}

BackendScriptRunnerDelegate::~BackendScriptRunnerDelegate() {}

void BackendScriptRunnerDelegate::AddBuiltinModule(const std::string& id,
                                                   ModuleGetter getter) {
  builtin_modules_[id] = base::Bind(getter);
}

void BackendScriptRunnerDelegate::AddBuiltinModule(
    const std::string& id, const ModuleGetterCallback& getter) {
  builtin_modules_[id] = getter;
}

v8::Handle<v8::ObjectTemplate> BackendScriptRunnerDelegate::GetGlobalTemplate(
    zarun::ScriptRunner* runner, v8::Isolate* isolate) {
  v8::Handle<v8::ObjectTemplate> templ =
      gin::ObjectTemplateBuilder(isolate).Build();
  return templ;
}

void BackendScriptRunnerDelegate::DidCreateContext(
    zarun::ScriptRunner* runner) {
  v8::Handle<v8::Context> context = runner->GetContextHolder()->context();
  ModuleRegistry* registry = ModuleRegistry::From(context);

  v8::Isolate* isolate = runner->GetContextHolder()->isolate();

  for (BuiltinModuleMap::const_iterator it = builtin_modules_.begin();
       it != builtin_modules_.end(); ++it) {
    registry->AddBuiltinModule(isolate, it->first, it->second.Run(isolate));
  }

  registry->LoadModule(
      isolate, zarun::Console::kModuleName,
      base::Bind(&InstallGlobalModule, runner, zarun::Console::kModuleName));

  registry->LoadModule(
      isolate, zarun::Process::kModuleName,
      base::Bind(&InstallGlobalModule, runner, zarun::Process::kModuleName));
}

void BackendScriptRunnerDelegate::UnhandledException(
    zarun::ScriptRunner* runner, gin::TryCatch& try_catch) {
  LOG(ERROR) << try_catch.GetStackTrace();
}

void BackendScriptRunnerDelegate::DidRunScript(zarun::ScriptRunner* runner) {}

void BackendScriptRunnerDelegate::ProcessResult(ScriptRunner* runner,
                                                v8::Local<v8::Value> result) {
  if (!runscript_callback_.is_null()) {
    runscript_callback_.Run(result);
  }
}
}
}  // namespace zarun::backend
