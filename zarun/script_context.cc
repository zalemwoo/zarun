/*
 * script_runner.cc
 *
 */

// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "zarun/script_context.h"

#include "gin/converter.h"
#include "gin/per_context_data.h"
#include "gin/public/context_holder.h"
#include "gin/try_catch.h"

#include "zarun/modules/module_registry.h"

#include "zarun/zarun_shell.h"

using gin::TryCatch;
using gin::ContextHolder;
using gin::PerContextData;

namespace zarun {

ScriptContextDelegate::ScriptContextDelegate() {
}

ScriptContextDelegate::~ScriptContextDelegate() {
}

v8::Handle<v8::ObjectTemplate> ScriptContextDelegate::GetGlobalTemplate(
    ScriptContext* runner,
    v8::Isolate* isolate) {
  return v8::Handle<v8::ObjectTemplate>();
}

void ScriptContextDelegate::DidCreateContext(ScriptContext* runner) {
}

void ScriptContextDelegate::WillRunScript(ScriptContext* runner) {
}

void ScriptContextDelegate::DidRunScript(ScriptContext* runner) {
}

void ScriptContextDelegate::UnhandledException(ScriptContext* runner,
                                               TryCatch& try_catch) {
  CHECK(false) << try_catch.GetStackTrace();
}

const std::string ScriptContext::kReplResultVariableName = "__repl_result__";

ScriptContext::ScriptContext(ScriptContextDelegate* delegate,
                             v8::Isolate* isolate)
    : delegate_(delegate), isolate_(isolate) {
  v8::Isolate::Scope isolate_scope(isolate);
  v8::HandleScope handle_scope(isolate);
  v8::Handle<v8::Context> v8_context = v8::Context::New(
      isolate, NULL, delegate_->GetGlobalTemplate(this, isolate));

  context_holder_.reset(new ContextHolder(isolate));
  context_holder_->SetContext(v8_context);
  PerContextData::From(v8_context)->set_runner(this);

  v8::Context::Scope scope(v8_context);
  delegate_->DidCreateContext(this);
}

ScriptContext::~ScriptContext() {
}

void ScriptContext::Run(const std::string& source,
                        const std::string& resource_name) {
  TryCatch try_catch;
  v8::Isolate* isolate = GetContextHolder()->isolate();
  v8::Handle<v8::Script> script =
      v8::Script::Compile(gin::StringToV8(isolate, source),
                          gin::StringToV8(isolate, resource_name));
  if (try_catch.HasCaught()) {
    delegate_->UnhandledException(this, try_catch);
    return;
  }
  Run(script);
}

v8::Handle<v8::Value> ScriptContext::Call(v8::Handle<v8::Function> function,
                                          v8::Handle<v8::Value> receiver,
                                          int argc,
                                          v8::Handle<v8::Value> argv[]) {
  TryCatch try_catch;
  delegate_->WillRunScript(this);

  v8::Handle<v8::Value> result = function->Call(receiver, argc, argv);

  delegate_->DidRunScript(this);
  if (try_catch.HasCaught())
    delegate_->UnhandledException(this, try_catch);

  return result;
}

ContextHolder* ScriptContext::GetContextHolder() {
  return context_holder_.get();
}

v8::Handle<v8::Context> ScriptContext::v8_context() const {
  ContextHolder* context_holder =
      (const_cast<ScriptContext*>(this))->GetContextHolder();
  if (context_holder) {
    return context_holder->context();
  }
  return v8::Local<v8::Context>();
}

void ScriptContext::Invalidate() {
  if (!is_valid())
    return;
  //	  if (module_system_)
  //	    module_system_->Invalidate();
  context_holder_.reset();
}

bool ScriptContext::is_valid() const {
  return !v8_context().IsEmpty();
}

void ScriptContext::Run(v8::Handle<v8::Script> script) {
  TryCatch try_catch;

  delegate_->WillRunScript(this);
  v8::Handle<v8::Value> result = script->Run();

  if (result.IsEmpty()) {
    if (try_catch.HasCaught()) {
      return delegate_->UnhandledException(this, try_catch);
    }
  }
  if (ZarunShell::Mode() == ShellMode::Repl) {
    v8::Isolate* isolate = GetContextHolder()->isolate();
    global()->SetHiddenValue(
        gin::StringToV8(isolate, ScriptContext::kReplResultVariableName),
        result);
  }
  delegate_->DidRunScript(this);
}

}  // namespace zarun
