/*
 * javascript_module_system.cc
 *
 */
// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "zarun/modules/javascript_module_system.h"

#include "base/bind.h"
#include "base/command_line.h"
#include "base/debug/trace_event.h"
#include "base/stl_util.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "zarun/console.h"
#include "zarun/script_context.h"
#include "gin/modules/module_registry.h"

#include "zarun/zarun_shell.h"

namespace zarun {

namespace {

const char* kModuleSystem = "module_system";
const char* kModulesField = "modules";

// Logs a fatal error for the calling context, with some added metadata about
// the context:
//  - Its type (blessed, unblessed, etc).
//  - Whether it's valid.
//  - The extension ID, if one exists.
//
// This will only actual be fatal in |batch| mode, in |repl| just output
// the message to stderr and continue.
void Fatal(ScriptContext* context, const std::string& message) {
  if (ZarunShell::Mode() == ShellMode::Repl)
    console::Error(context->isolate()->GetCallingContext(), message);
  else
    console::Fatal(context->isolate()->GetCallingContext(), message);
}

void Warn(v8::Isolate* isolate, const std::string& message) {
  console::Warn(isolate->GetCallingContext(), message);
}

// Default exception handler which logs the exception.
class DefaultExceptionHandler
    : public JavaScriptModuleSystem::ExceptionHandler {
 public:
  explicit DefaultExceptionHandler(ScriptContext* context)
      : context_(context) {}

  // Fatally dumps the debug info from |try_catch| to the console.
  // Make sure this is never used for exceptions that originate in external
  // code!
  void HandleUncaughtException(const v8::TryCatch& try_catch) override {
    v8::HandleScope handle_scope(context_->isolate());
    std::string stack_trace = "<stack trace unavailable>";
    if (!try_catch.StackTrace().IsEmpty()) {
      v8::String::Utf8Value stack_value(try_catch.StackTrace());
      if (*stack_value)
        stack_trace.assign(*stack_value, stack_value.length());
      else
        stack_trace = "<could not convert stack trace to string>";
    }
    Fatal(context_, CreateExceptionString(try_catch) + "{" + stack_trace + "}");
  }

 private:
  ScriptContext* context_;
};

}  // namespace

std::string JavaScriptModuleSystem::ExceptionHandler::CreateExceptionString(
    const v8::TryCatch& try_catch) {
  v8::Handle<v8::Message> message(try_catch.Message());
  if (message.IsEmpty()) {
    return "try_catch has no message";
  }

  std::string resource_name = "<unknown resource>";
  if (!message->GetScriptOrigin().ResourceName().IsEmpty()) {
    v8::String::Utf8Value resource_name_v8(
        message->GetScriptOrigin().ResourceName());
    resource_name.assign(*resource_name_v8, resource_name_v8.length());
  }

  std::string error_message = "<no error message>";
  if (!message->Get().IsEmpty()) {
    v8::String::Utf8Value error_message_v8(message->Get());
    error_message.assign(*error_message_v8, error_message_v8.length());
  }

  return base::StringPrintf("%s:%d: %s", resource_name.c_str(),
                            message->GetLineNumber(), error_message.c_str());
}

JavaScriptModuleSystem::JavaScriptModuleSystem(ScriptContext* context,
                                               SourceMap* source_map)
    : ObjectBackedNativeModule(context),
      context_(context),
      source_map_(source_map),
      natives_enabled_(0),
      exception_handler_(new DefaultExceptionHandler(context)),
      weak_factory_(this) {
  RouteFunction("require", base::Bind(&JavaScriptModuleSystem::RequireForJs,
                                      base::Unretained(this)));
  RouteFunction("requireNative",
                base::Bind(&JavaScriptModuleSystem::RequireNative,
                           base::Unretained(this)));
  RouteFunction("requireAsync",
                base::Bind(&JavaScriptModuleSystem::RequireAsync,
                           base::Unretained(this)));
  RouteFunction("privates", base::Bind(&JavaScriptModuleSystem::Private,
                                       base::Unretained(this)));

  v8::Handle<v8::Object> global(context->v8_context()->Global());
  v8::Isolate* isolate = context->isolate();
  global->SetHiddenValue(v8::String::NewFromUtf8(isolate, kModulesField),
                         v8::Object::New(isolate));
  global->SetHiddenValue(v8::String::NewFromUtf8(isolate, kModuleSystem),
                         v8::External::New(isolate, this));

  gin::ModuleRegistry::From(context->v8_context())->AddObserver(this);
}

JavaScriptModuleSystem::~JavaScriptModuleSystem() {
  Invalidate();
}

void JavaScriptModuleSystem::Invalidate() {
  if (!is_valid())
    return;

  // Clear the module system properties from the global context. It's polite,
  // and we use this as a signal in lazy handlers that we no longer exist.
  {
    v8::HandleScope scope(GetIsolate());
    v8::Handle<v8::Object> global = context()->v8_context()->Global();
    global->DeleteHiddenValue(
        v8::String::NewFromUtf8(GetIsolate(), kModulesField));
    global->DeleteHiddenValue(
        v8::String::NewFromUtf8(GetIsolate(), kModuleSystem));
  }

  // Invalidate all of the successfully required handlers we own.
  for (NativeModuleMap::iterator it = native_module_map_.begin();
       it != native_module_map_.end(); ++it) {
    it->second->Invalidate();
  }

  ObjectBackedNativeModule::Invalidate();
}

JavaScriptModuleSystem::NativesEnabledScope::NativesEnabledScope(
    JavaScriptModuleSystem* module_system)
    : module_system_(module_system) {
  module_system_->natives_enabled_++;
}

JavaScriptModuleSystem::NativesEnabledScope::~NativesEnabledScope() {
  module_system_->natives_enabled_--;
  CHECK_GE(module_system_->natives_enabled_, 0);
}

void JavaScriptModuleSystem::HandleException(const v8::TryCatch& try_catch) {
  exception_handler_->HandleUncaughtException(try_catch);
}

v8::Handle<v8::Value> JavaScriptModuleSystem::Require(
    const std::string& module_name) {
  v8::EscapableHandleScope handle_scope(GetIsolate());
  return handle_scope.Escape(RequireForJsInner(
      v8::String::NewFromUtf8(GetIsolate(), module_name.c_str())));
}

void JavaScriptModuleSystem::RequireForJs(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Handle<v8::String> module_name = args[0]->ToString(args.GetIsolate());
  args.GetReturnValue().Set(RequireForJsInner(module_name));
}

v8::Local<v8::Value> JavaScriptModuleSystem::RequireForJsInner(
    v8::Handle<v8::String> module_name) {
  v8::EscapableHandleScope handle_scope(GetIsolate());
  v8::Context::Scope context_scope(context()->v8_context());

  v8::Handle<v8::Object> global(context()->v8_context()->Global());

  // The module system might have been deleted. This can happen if a different
  // context keeps a reference to us, but our frame is destroyed (e.g.
  // background page keeps reference to chrome object in a closed popup).
  v8::Handle<v8::Value> modules_value = global->GetHiddenValue(
      v8::String::NewFromUtf8(GetIsolate(), kModulesField));
  if (modules_value.IsEmpty() || modules_value->IsUndefined()) {
    Warn(GetIsolate(), "Module system no longer exists");
    return v8::Undefined(GetIsolate());
  }

  v8::Handle<v8::Object> modules(v8::Handle<v8::Object>::Cast(modules_value));
  v8::Local<v8::Value> exports(modules->Get(module_name));
  if (!exports->IsUndefined())
    return handle_scope.Escape(exports);

  exports = LoadModule(*v8::String::Utf8Value(module_name));
  modules->Set(module_name, exports);
  return handle_scope.Escape(exports);
}

v8::Local<v8::Value> JavaScriptModuleSystem::CallModuleMethod(
    const std::string& module_name,
    const std::string& method_name) {
  v8::EscapableHandleScope handle_scope(GetIsolate());
  v8::Handle<v8::Value> no_args;
  return handle_scope.Escape(
      CallModuleMethod(module_name, method_name, 0, &no_args));
}

v8::Local<v8::Value> JavaScriptModuleSystem::CallModuleMethod(
    const std::string& module_name,
    const std::string& method_name,
    std::vector<v8::Handle<v8::Value> >* args) {
  return CallModuleMethod(module_name, method_name, args->size(),
                          vector_as_array(args));
}

v8::Local<v8::Value> JavaScriptModuleSystem::CallModuleMethod(
    const std::string& module_name,
    const std::string& method_name,
    int argc,
    v8::Handle<v8::Value> argv[]) {
  TRACE_EVENT2("v8", "v8.callModuleMethod", "module_name", module_name,
               "method_name", method_name);

  v8::EscapableHandleScope handle_scope(GetIsolate());
  v8::Context::Scope context_scope(context()->v8_context());

  v8::Local<v8::Value> module;
  {
    NativesEnabledScope natives_enabled(this);
    module = RequireForJsInner(
        v8::String::NewFromUtf8(GetIsolate(), module_name.c_str()));
  }

  if (module.IsEmpty() || !module->IsObject()) {
    Fatal(context_,
          "Failed to get module " + module_name + " to call " + method_name);
    return handle_scope.Escape(
        v8::Local<v8::Primitive>(v8::Undefined(GetIsolate())));
  }

  v8::Local<v8::Value> value = v8::Handle<v8::Object>::Cast(module)->Get(
      v8::String::NewFromUtf8(GetIsolate(), method_name.c_str()));
  if (value.IsEmpty() || !value->IsFunction()) {
    Fatal(context_, module_name + "." + method_name + " is not a function");
    return handle_scope.Escape(
        v8::Local<v8::Primitive>(v8::Undefined(GetIsolate())));
  }

  v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(value);
  v8::Local<v8::Value> result;
  {
    v8::TryCatch try_catch;
    try_catch.SetCaptureMessage(true);
    result = context_->CallFunction(func, argc, argv);
    if (try_catch.HasCaught())
      HandleException(try_catch);
  }
  return handle_scope.Escape(result);
}

void JavaScriptModuleSystem::RegisterNativeModule(
    const std::string& name,
    scoped_ptr<NativeJavaScriptModule> native_module) {
  native_module_map_[name] =
      linked_ptr<NativeJavaScriptModule>(native_module.release());
}

void JavaScriptModuleSystem::OverrideNativeModuleForTest(
    const std::string& name) {
  overridden_native_handlers_.insert(name);
}

v8::Handle<v8::Value> JavaScriptModuleSystem::RunString(
    const std::string& code,
    const std::string& name) {
  v8::EscapableHandleScope handle_scope(GetIsolate());
  v8::Local<v8::Value> result =
      RunString(v8::String::NewFromUtf8(GetIsolate(), code.c_str()),
                v8::String::NewFromUtf8(GetIsolate(), name.c_str()));
  return handle_scope.Escape(result);
}

v8::Handle<v8::Value> JavaScriptModuleSystem::RunString(
    v8::Handle<v8::String> code,
    v8::Handle<v8::String> name) {
  v8::EscapableHandleScope handle_scope(GetIsolate());
  v8::Context::Scope context_scope(context()->v8_context());

  v8::TryCatch try_catch;
  try_catch.SetCaptureMessage(true);
  v8::Handle<v8::Script> script(v8::Script::Compile(code, name));
  if (try_catch.HasCaught()) {
    HandleException(try_catch);
    return v8::Undefined(GetIsolate());
  }

  v8::Local<v8::Value> result = script->Run();
  if (try_catch.HasCaught()) {
    HandleException(try_catch);
    return v8::Undefined(GetIsolate());
  }

  return handle_scope.Escape(result);
}

v8::Handle<v8::Value> JavaScriptModuleSystem::GetSource(
    const std::string& module_name) {
  v8::EscapableHandleScope handle_scope(GetIsolate());
  if (!source_map_->Contains(module_name))
    return v8::Undefined(GetIsolate());
  return handle_scope.Escape(
      v8::Local<v8::Value>(source_map_->GetSource(GetIsolate(), module_name)));
}

void JavaScriptModuleSystem::RequireNative(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  CHECK_EQ(1, args.Length());
  std::string native_name = *v8::String::Utf8Value(args[0]);
  args.GetReturnValue().Set(RequireNativeFromString(native_name));
}

v8::Handle<v8::Value> JavaScriptModuleSystem::RequireNativeFromString(
    const std::string& native_name) {
  if (natives_enabled_ == 0) {
    // HACK: if in test throw exception so that we can test the natives-disabled
    // logic; however, under normal circumstances, this is programmer error so
    // we could crash.
    if (exception_handler_) {
      return GetIsolate()->ThrowException(
          v8::String::NewFromUtf8(GetIsolate(), "Natives disabled"));
    }
    Fatal(context_, "Natives disabled for requireNative(" + native_name + ")");
    return v8::Undefined(GetIsolate());
  }

  if (overridden_native_handlers_.count(native_name) > 0u) {
    return RequireForJsInner(
        v8::String::NewFromUtf8(GetIsolate(), native_name.c_str()));
  }

  NativeModuleMap::iterator i = native_module_map_.find(native_name);
  if (i == native_module_map_.end()) {
    Fatal(context_,
          "Couldn't find native for requireNative(" + native_name + ")");
    return v8::Undefined(GetIsolate());
  }
  return i->second->NewInstance();
}

void JavaScriptModuleSystem::RequireAsync(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  CHECK_EQ(1, args.Length());
  std::string module_name = *v8::String::Utf8Value(args[0]);
  v8::Handle<v8::Promise::Resolver> resolver(
      v8::Promise::Resolver::New(GetIsolate()));
  args.GetReturnValue().Set(resolver->GetPromise());
  scoped_ptr<v8::UniquePersistent<v8::Promise::Resolver> > persistent_resolver(
      new v8::UniquePersistent<v8::Promise::Resolver>(GetIsolate(), resolver));
  gin::ModuleRegistry* module_registry =
      gin::ModuleRegistry::From(context_->v8_context());
  if (!module_registry) {
    Warn(GetIsolate(), "Module system no longer exists");
    resolver->Reject(v8::Exception::Error(v8::String::NewFromUtf8(
        GetIsolate(), "Module system no longer exists")));
    return;
  }
  module_registry->LoadModule(
      GetIsolate(), module_name,
      base::Bind(&JavaScriptModuleSystem::OnModuleLoaded,
                 weak_factory_.GetWeakPtr(),
                 base::Passed(&persistent_resolver)));
  if (module_registry->available_modules().count(module_name) == 0)
    LoadModule(module_name);
}

v8::Handle<v8::String> JavaScriptModuleSystem::WrapSource(
    v8::Handle<v8::String> source) {
  v8::EscapableHandleScope handle_scope(GetIsolate());
  // Keep in order with the arguments in RequireForJsInner.
  v8::Handle<v8::String> left = v8::String::NewFromUtf8(
      GetIsolate(),
      "(function(define, require, requireNative, requireAsync, exports, "
      "console, privates,"
      "$Array, $Function, $JSON, $Object, $RegExp, $String, $Error) {"
      "'use strict';");
  v8::Handle<v8::String> right = v8::String::NewFromUtf8(GetIsolate(), "\n})");
  return handle_scope.Escape(v8::Local<v8::String>(
      v8::String::Concat(left, v8::String::Concat(source, right))));
}

void JavaScriptModuleSystem::Private(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  CHECK_EQ(1, args.Length());
  CHECK(args[0]->IsObject());
  CHECK(!args[0]->IsNull());
  v8::Local<v8::Object> obj = args[0].As<v8::Object>();
  v8::Local<v8::String> privates_key =
      v8::String::NewFromUtf8(GetIsolate(), "privates");
  v8::Local<v8::Value> privates = obj->GetHiddenValue(privates_key);
  if (privates.IsEmpty()) {
    privates = v8::Object::New(args.GetIsolate());
    if (privates.IsEmpty()) {
      GetIsolate()->ThrowException(
          v8::String::NewFromUtf8(GetIsolate(), "Failed to create privates"));
      return;
    }
    obj->SetHiddenValue(privates_key, privates);
  }
  args.GetReturnValue().Set(privates);
}

v8::Handle<v8::Value> JavaScriptModuleSystem::LoadModule(
    const std::string& module_name) {
  v8::EscapableHandleScope handle_scope(GetIsolate());
  v8::Context::Scope context_scope(context()->v8_context());

  v8::Handle<v8::Value> source(GetSource(module_name));
  if (source.IsEmpty() || source->IsUndefined()) {
    Fatal(context_, "No source for require(" + module_name + ")");
    return v8::Undefined(GetIsolate());
  }
  v8::Handle<v8::String> wrapped_source(
      WrapSource(v8::Handle<v8::String>::Cast(source)));
  // Modules are wrapped in (function(){...}) so they always return functions.
  v8::Handle<v8::Value> func_as_value =
      RunString(wrapped_source,
                v8::String::NewFromUtf8(GetIsolate(), module_name.c_str()));
  if (func_as_value.IsEmpty() || func_as_value->IsUndefined()) {
    Fatal(context_, "Bad source for require(" + module_name + ")");
    return v8::Undefined(GetIsolate());
  }

  v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(func_as_value);

  v8::Handle<v8::Object> define_object = v8::Object::New(GetIsolate());
  gin::ModuleRegistry::InstallGlobals(GetIsolate(), define_object);

  v8::Local<v8::Value> exports = v8::Object::New(GetIsolate());
  v8::Handle<v8::Object> natives(NewInstance());
  CHECK(!natives.IsEmpty());  // this can happen if v8 has issues

  // These must match the argument order in WrapSource.
  v8::Handle<v8::Value> args[] = {
      // AMD.
      define_object->Get(v8::String::NewFromUtf8(GetIsolate(), "define")),
      // CommonJS.
      natives->Get(v8::String::NewFromUtf8(GetIsolate(), "require",
                                           v8::String::kInternalizedString)),
      natives->Get(v8::String::NewFromUtf8(GetIsolate(), "requireNative",
                                           v8::String::kInternalizedString)),
      natives->Get(v8::String::NewFromUtf8(GetIsolate(), "requireAsync",
                                           v8::String::kInternalizedString)),
      exports,
      // Libraries that we magically expose to every module.
      console::AsV8Object(GetIsolate()),
      natives->Get(v8::String::NewFromUtf8(GetIsolate(), "privates",
                                           v8::String::kInternalizedString)),
      // Each safe builtin. Keep in order with the arguments in WrapSource.
      context_->safe_builtins()->GetArray(),
      context_->safe_builtins()->GetFunction(),
      context_->safe_builtins()->GetJSON(),
      context_->safe_builtins()->GetObjekt(),
      context_->safe_builtins()->GetRegExp(),
      context_->safe_builtins()->GetString(),
      context_->safe_builtins()->GetError(),
  };
  {
    v8::TryCatch try_catch;
    try_catch.SetCaptureMessage(true);
    context_->CallFunction(func, arraysize(args), args);
    if (try_catch.HasCaught()) {
      HandleException(try_catch);
      return v8::Undefined(GetIsolate());
    }
  }
  return handle_scope.Escape(exports);
}

void JavaScriptModuleSystem::OnDidAddPendingModule(
    const std::string& id,
    const std::vector<std::string>& dependencies) {
  if (!source_map_->Contains(id))
    return;

  gin::ModuleRegistry* registry =
      gin::ModuleRegistry::From(context_->v8_context());
  DCHECK(registry);
  for (std::vector<std::string>::const_iterator it = dependencies.begin();
       it != dependencies.end(); ++it) {
    if (registry->available_modules().count(*it) == 0)
      LoadModule(*it);
  }
  registry->AttemptToLoadMoreModules(GetIsolate());
}

void JavaScriptModuleSystem::OnModuleLoaded(
    scoped_ptr<v8::UniquePersistent<v8::Promise::Resolver> > resolver,
    v8::Handle<v8::Value> value) {
  if (!is_valid())
    return;
  v8::HandleScope handle_scope(GetIsolate());
  v8::Handle<v8::Promise::Resolver> resolver_local(
      v8::Local<v8::Promise::Resolver>::New(GetIsolate(), *resolver));
  resolver_local->Resolve(value);
}

}  // namespace zarun
