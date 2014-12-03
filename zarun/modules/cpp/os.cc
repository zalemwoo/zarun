/*
 * os.cc
 *
 */

#include "zarun/modules/cpp/os.h"

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
#include "zarun/zarun_version.h"
#include "zarun/script_context.h"

#include "zarun/utils/path_util.h"

#include "zarun/modules/cpp/internal/system_env.h"

namespace zarun {

namespace {

// os.version
std::string GetVersion() {
  return std::string(ZARUN_VERSION_FULL);
}

// os.versions
v8::Handle<v8::Object> GetVersions(v8::Isolate* isolate) {
  v8::Local<v8::Object> versions_obj = v8::Object::New(isolate);
  versions_obj->ForceSet(
      gin::StringToV8(isolate, "v8"),
      gin::StringToV8(isolate, base::StringPiece(v8::V8::GetVersion())),
      v8::ReadOnly);

  return versions_obj;
}

// os.argv
std::vector<std::string> GetArgv() {
  std::vector<std::string> argv =
      base::CommandLine::ForCurrentProcess()->argv();
  return argv;
}

// os.execPath
std::string GetExecutablePath() {
  return zarun::util::ExecutionPath().AsUTF8Unsafe();
}

// os.features
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

// os.abort
void AbortCallback(const v8::FunctionCallbackInfo<v8::Value>& info) {
  base::debug::StackTrace().Print();
  abort();
}

// os.chdir
void ChdirCallback(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() != 1 || !info[0]->IsString()) {
    return info.GetReturnValue().Set(
        v8::Boolean::New(info.GetIsolate(), false));
  }
  std::string path = gin::V8ToString(info[0]);
  bool success = base::SetCurrentDirectory(base::FilePath(path));
  info.GetReturnValue().Set(v8::Boolean::New(info.GetIsolate(), success));
}

// os.cwd
void CwdCallback(const v8::FunctionCallbackInfo<v8::Value>& info) {
  base::FilePath cwd;
  if (!base::GetCurrentDirectory(&cwd)) {
    info.GetReturnValue().SetUndefined();
  }
  return info.GetReturnValue().Set(
      gin::StringToV8(info.GetIsolate(), cwd.AsUTF8Unsafe()));
}

void ModulesCallback(const v8::FunctionCallbackInfo<v8::Value>& info) {
  v8::Isolate* isolate = info.GetIsolate();
  v8::HandleScope scope(isolate);
  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  v8::Handle<v8::Object> global(context->Global());
  v8::Local<v8::Value> modules =
      global->GetHiddenValue(v8::String::NewFromUtf8(isolate, "modules"));
  info.GetReturnValue().Set(modules);
}

}  // namespace

OSNative::OSNative(ScriptContext* context) : ObjectBackedNativeModule(context) {
  RouteFunction("cwd", base::Bind(&CwdCallback));
  RouteFunction("abort", base::Bind(&AbortCallback));
  RouteFunction("chdir", base::Bind(&ChdirCallback));
  RouteFunction("modules", base::Bind(&ModulesCallback));
}

OSNative::~OSNative() {
  Invalidate();
}

v8::Handle<v8::Object> OSNative::NewInstance() {
  v8::Isolate* isolate = this->context()->isolate();
  v8::EscapableHandleScope scope(isolate);

  v8::Local<v8::Object> module = ObjectBackedNativeModule::NewInstance();

  module->ForceSet(gin::StringToV8(isolate, "version"),
                   gin::StringToV8(isolate, GetVersion()),
                   v8::PropertyAttribute::ReadOnly);
  module->ForceSet(gin::StringToV8(isolate, "versions"), GetVersions(isolate),
                   v8::PropertyAttribute::ReadOnly);
  module->ForceSet(gin::StringToV8(isolate, "features"), GetFeatures(isolate),
                   v8::PropertyAttribute::ReadOnly);
  module->ForceSet(gin::StringToV8(isolate, "argv"),
                   gin::ConvertToV8(isolate, GetArgv()),
                   v8::PropertyAttribute::ReadOnly);
  module->ForceSet(gin::StringToV8(isolate, "execPath"),
                   gin::StringToV8(isolate, GetExecutablePath()),
                   v8::PropertyAttribute::ReadOnly);
  module->ForceSet(gin::StringToV8(isolate, "name"),
                   gin::StringToV8(isolate, "javascript"),
                   v8::PropertyAttribute::ReadOnly);
  module->ForceSet(
      gin::StringToV8(isolate, "platform"),
      gin::StringToV8(
          isolate,
          std::string(zarun::platform::SystemInfo::OperatingSystemCodeName())),
      v8::PropertyAttribute::ReadOnly);
  module->ForceSet(
      gin::StringToV8(isolate, "arch"),
      gin::StringToV8(
          isolate,
          std::string(zarun::platform::SystemInfo::PlatformArchitecture())),
      v8::PropertyAttribute::ReadOnly);
  module->ForceSet(
      gin::StringToV8(isolate, "is_posix"),
      gin::ConvertToV8(isolate, zarun::platform::SystemInfo::IsPosix()),
      v8::PropertyAttribute::ReadOnly);

  module->ForceSet(gin::StringToV8(isolate, "environ"),
                   zarun::internal::CreateSystemEnv(isolate),
                   v8::PropertyAttribute::ReadOnly);
  module->ForceSet(gin::StringToV8(isolate, "pid"),
                   gin::ConvertToV8(isolate, base::GetCurrentProcId()),
                   v8::PropertyAttribute::ReadOnly);

  return scope.Escape(module);
}

}  // namespace zarun
