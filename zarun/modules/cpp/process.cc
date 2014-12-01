/*
 * process.cc
 *
 */

#include "zarun/modules/cpp/process.h"

#include <stdlib.h>

#include <set>
#include <vector>
#include <string>

#include "base/logging.h"
#include "base/command_line.h"
#include "base/debug/stack_trace.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "base/process/process.h"
#include "base/process/process_handle.h"
#include "base/process/kill.h"
#include "base/files/file_util.h"
#include "gin/converter.h"
#include "v8/include/v8.h"

#include "platform/base/system_info.h"
#include "platform/base/environment.h"
#include "zarun/zarun_version.h"
#include "zarun/script_context.h"

#include "zarun/utils/path_util.h"

namespace zarun {

namespace {

// process.version
std::string GetVersion() {
  return std::string(ZARUN_VERSION_FULL);
}

// process.versions
v8::Handle<v8::Object> GetVersions(v8::Isolate* isolate) {
  v8::Local<v8::Object> versions_obj = v8::Object::New(isolate);
  versions_obj->ForceSet(
      gin::StringToV8(isolate, "v8"),
      gin::StringToV8(isolate, base::StringPiece(v8::V8::GetVersion())),
      v8::ReadOnly);

  return versions_obj;
}

// process.argv
std::vector<std::string> GetArgv() {
  std::vector<std::string> argv =
      base::CommandLine::ForCurrentProcess()->argv();
  return argv;
}

// process.execPath
std::string GetExecutablePath() {
  return zarun::util::ExecutionPath().AsUTF8Unsafe();
}

// process.features
v8::Handle<v8::Object> GetFeatures(v8::Isolate* isolate) {
  v8::EscapableHandleScope scope(isolate);
  v8::Local<v8::Object> features = v8::Object::New(isolate);

  v8::Handle<v8::Value> debug = gin::ConvertToV8(isolate,
#if !defined(NDEBUG)
                                                 true
#else
                                                 false
#endif
                                                 );
  features->Set(gin::StringToV8(isolate, "debug"), debug);
  return scope.Escape(features);
}

// process.abort
void AbortCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
  base::debug::StackTrace().Print();
  abort();
}

// process.chdir
void ChdirCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
  if (args.Length() != 1 || !args[0]->IsString()) {
    return args.GetReturnValue().Set(
        v8::Boolean::New(args.GetIsolate(), false));
  }
  std::string path = gin::V8ToString(args[0]);
  bool success = base::SetCurrentDirectory(base::FilePath(path));
  args.GetReturnValue().Set(v8::Boolean::New(args.GetIsolate(), success));
}

// process.cwd
void CwdCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
  base::FilePath cwd;
  if (!base::GetCurrentDirectory(&cwd)) {
    args.GetReturnValue().SetUndefined();
  }
  return args.GetReturnValue().Set(
      gin::StringToV8(args.GetIsolate(), cwd.AsUTF8Unsafe()));
}

// process.env
void EnvGetter(v8::Local<v8::String> property,
               const v8::PropertyCallbackInfo<v8::Value>& info) {
  std::string prop = gin::V8ToString(property);
  v8::Isolate* isolate = info.GetIsolate();
  zarun::platform::Environment env;
  std::string env_value;
  bool ret = env.GetVar(prop.c_str(), &env_value);

  if (ret) {
    info.GetReturnValue().Set(gin::StringToV8(isolate, env_value));
    return;
  }
  // Not found.  Fetch from prototype.
  info.GetReturnValue().Set(info.Data().As<v8::Object>()->Get(property));
}

void EnvSetter(v8::Local<v8::String> property,
               v8::Local<v8::Value> value,
               const v8::PropertyCallbackInfo<v8::Value>& info) {
  std::string prop = gin::V8ToString(property);
  zarun::platform::Environment env;
  std::string env_value(*v8::String::Utf8Value(value));
  env.SetVar(prop.c_str(), env_value);

  // Whether it worked or not, always return rval.
  info.GetReturnValue().Set(value);
}

void EnvQuery(v8::Local<v8::String> property,
              const v8::PropertyCallbackInfo<v8::Integer>& info) {
  std::string prop = gin::V8ToString(property);
  zarun::platform::Environment env;
  bool ret = env.HasVar(prop.c_str());
  if (ret)
    info.GetReturnValue().Set(0);
}

void EnvDeleter(v8::Local<v8::String> property,
                const v8::PropertyCallbackInfo<v8::Boolean>& info) {
  std::string prop = gin::V8ToString(property);
  zarun::platform::Environment env;
  bool ret = env.UnSetVar(prop.c_str());
  info.GetReturnValue().Set(ret);
}

void EnvEnumerator(const v8::PropertyCallbackInfo<v8::Array>& info) {
  v8::Isolate* isolate = info.GetIsolate();
  std::vector<std::string> envs = zarun::platform::Environment::GetAll();
  size_t size = envs.size();
  v8::Local<v8::Array> envarr = v8::Array::New(isolate, size);
  for (size_t i = 0; i < size; ++i) {
    v8::Local<v8::String> name = gin::StringToV8(isolate, envs[i]);
    envarr->Set(i, name);
  }
  info.GetReturnValue().Set(envarr);
}

v8::Handle<v8::Object> GetEnvironment(v8::Isolate* isolate) {
  v8::Local<v8::ObjectTemplate> env_templ = v8::ObjectTemplate::New();
  env_templ->SetNamedPropertyHandler(EnvGetter, EnvSetter, EnvQuery, EnvDeleter,
                                     EnvEnumerator, v8::Object::New(isolate));
  return env_templ->NewInstance();
}

void ModulesCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope scope(isolate);
  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  v8::Handle<v8::Object> global(context->Global());
  v8::Local<v8::Value> modules =
      global->GetHiddenValue(v8::String::NewFromUtf8(isolate, "modules"));
  args.GetReturnValue().Set(modules);
}

}  // namespace

ProcessNative::ProcessNative(ScriptContext* context)
    : ObjectBackedNativeModule(context) {
  RouteFunction("cwd", base::Bind(&CwdCallback));
  RouteFunction("abort", base::Bind(&AbortCallback));
  RouteFunction("chdir", base::Bind(&ChdirCallback));
  RouteFunction("modules", base::Bind(&ModulesCallback));
}

ProcessNative::~ProcessNative() {
  Invalidate();
}

v8::Handle<v8::Object> ProcessNative::NewInstance() {
  ScriptContext* context = this->context();
  v8::Isolate* isolate = context->isolate();
  v8::EscapableHandleScope scope(isolate);

  v8::Local<v8::Object> process = ObjectBackedNativeModule::NewInstance();

  process->ForceSet(gin::StringToV8(isolate, "version"),
                    gin::StringToV8(isolate, GetVersion()),
                    v8::PropertyAttribute::ReadOnly);
  process->ForceSet(gin::StringToV8(isolate, "versions"), GetVersions(isolate),
                    v8::PropertyAttribute::ReadOnly);
  process->ForceSet(gin::StringToV8(isolate, "features"), GetFeatures(isolate),
                    v8::PropertyAttribute::ReadOnly);
  process->ForceSet(gin::StringToV8(isolate, "argv"),
                    gin::ConvertToV8(isolate, GetArgv()),
                    v8::PropertyAttribute::ReadOnly);
  process->ForceSet(gin::StringToV8(isolate, "execPath"),
                    gin::StringToV8(isolate, GetExecutablePath()),
                    v8::PropertyAttribute::ReadOnly);
  process->ForceSet(
      gin::StringToV8(isolate, "platform"),
      gin::StringToV8(
          isolate,
          std::string(zarun::platform::SystemInfo::OperatingSystemCodeName())),
      v8::PropertyAttribute::ReadOnly);
  process->ForceSet(
      gin::StringToV8(isolate, "arch"),
      gin::StringToV8(
          isolate,
          std::string(zarun::platform::SystemInfo::PlatformArchitecture())),
      v8::PropertyAttribute::ReadOnly);
  process->ForceSet(gin::StringToV8(isolate, "env"), GetEnvironment(isolate),
                    v8::PropertyAttribute::ReadOnly);
  process->ForceSet(gin::StringToV8(isolate, "pid"),
                    gin::ConvertToV8(isolate, base::GetCurrentProcId()),
                    v8::PropertyAttribute::ReadOnly);

  return scope.Escape(process);
}

}  // namespace zarun
