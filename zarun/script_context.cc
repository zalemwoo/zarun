/*
 * script_runner.cc
 *
 */

// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "zarun/script_context.h"

#include "base/logging.h"
#include "gin/converter.h"
#include "gin/try_catch.h"

#include "zarun/modules/module_registry.h"

#include "gin/per_context_data.h"
#include "zarun/zarun_shell.h"

using gin::TryCatch;
using gin::ContextHolder;

namespace zarun {

ScriptContextDelegate::ScriptContextDelegate() {
}

ScriptContextDelegate::~ScriptContextDelegate() {
}

v8::Handle<v8::ObjectTemplate> ScriptContextDelegate::GetGlobalTemplate(
    v8::Isolate* isolate) {
  return v8::Handle<v8::ObjectTemplate>();
}

void ScriptContextDelegate::DidCreateContext(ScriptContext* context) {
}

void ScriptContextDelegate::WillRunScript(ScriptContext* context) {
}

void ScriptContextDelegate::DidRunScript(ScriptContext* context) {
}

void ScriptContextDelegate::UnhandledException(ScriptContext* context,
                                               TryCatch& try_catch) {
  CHECK(false) << try_catch.GetStackTrace();
}

void ScriptContextDelegate::DidCreateEnvironment(ScriptContext* context) {
}

// ScriptContext
const std::string ScriptContext::kReplResultVariableName = "__repl_result__";

ScriptContext::ScriptContext(ScriptContextDelegate* delegate,
                             const v8::Handle<v8::Context>& v8_context)
    : delegate_(delegate),
      v8_context_(v8_context),
      safe_builtins_(this),
      isolate_(v8_context->GetIsolate()) {
  VLOG(1) << "Created context:\n";
  gin::PerContextData::From(v8_context)->set_runner(this);
  v8::Context::Scope scope(v8_context);
  delegate_->DidCreateContext(this);
}

ScriptContext::~ScriptContext() {
  VLOG(1) << "Destroyed context for extension\n";
  Invalidate();
}

v8::Local<v8::Value> ScriptContext::CallFunction(
    v8::Handle<v8::Function> function,
    int argc,
    v8::Handle<v8::Value> argv[]) const {
  v8::EscapableHandleScope handle_scope(isolate());
  v8::Context::Scope scope(v8_context());

  if (!is_valid()) {
    return handle_scope.Escape(
        v8::Local<v8::Primitive>(v8::Undefined(isolate())));
  }

  v8::Handle<v8::Object> global = v8_context()->Global();
  return handle_scope.Escape(function->Call(global, argc, argv));
}

void ScriptContext::Invalidate() {
  if (!is_valid())
    return;
  if (module_system_)
    module_system_->Invalidate();
  v8_context_.reset();
}

bool ScriptContext::is_valid() const {
  return !v8_context_.IsEmpty();
}

v8::Handle<v8::Context> ScriptContext::v8_context() const {
  return v8_context_.NewHandle(isolate());
}

void ScriptContext::Run(const std::string& source,
                        const std::string& resource_name) {
  delegate_->WillRunScript(this);
  v8::Handle<v8::Value> result =
      module_system_->RunString(source, resource_name);

  if (ZarunShell::Mode() == ShellMode::Repl) {
    v8::Isolate* isolate = GetContextHolder()->isolate();
    v8_context()->Global()->SetHiddenValue(
        gin::StringToV8(isolate, ScriptContext::kReplResultVariableName),
        result);
  }
  delegate_->DidRunScript(this);
}

v8::Handle<v8::Value> ScriptContext::Call(v8::Handle<v8::Function> function,
                                          v8::Handle<v8::Value> receiver,
                                          int argc,
                                          v8::Handle<v8::Value> argv[]) {
  TryCatch try_catch;
  delegate_->WillRunScript(this);

  v8::Handle<v8::Value> result = CallFunction(function, argc, argv);

  delegate_->DidRunScript(this);
  if (try_catch.HasCaught())
    delegate_->UnhandledException(this, try_catch);

  return result;
}

ContextHolder* ScriptContext::GetContextHolder() {
  v8::HandleScope handle_scope(isolate());
  return gin::PerContextData::From(v8_context())->context_holder();
}

}  // namespace zarun
