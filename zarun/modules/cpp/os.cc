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
std::string GetSelfVersion() {
  return std::string(ZARUN_VERSION_FULL);
}

// os.versions
v8::Handle<v8::Object> GetThirdPartyVersions(v8::Isolate* isolate) {
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
void AbortCallback() {
  abort();
}

// os.chdir
void ChangeDirectoryCallback(gin::Arguments* args) {
  std::string path;
  if (args->Length() != 1 || !args->GetNext(&path)) {
    return args->Return(false);
  }
  bool success = base::SetCurrentDirectory(base::FilePath(path));
  args->Return(success);
}

// os.getcwd
void GetCurrentWorkingDirectoryCallback(gin::Arguments* args) {
  base::FilePath cwd;
  if (!base::GetCurrentDirectory(&cwd)) {
    args->Return((v8::Handle<v8::Value>)(v8::Undefined(args->isolate())));
  }
  args->Return(cwd.AsUTF8Unsafe());
}

// os.modules
void ModulesCallback(gin::Arguments* args) {
  v8::Isolate* isolate = args->isolate();
  v8::HandleScope scope(isolate);
  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  v8::Handle<v8::Object> global(context->Global());
  v8::Handle<v8::Value> modules =
      global->GetHiddenValue(v8::String::NewFromUtf8(isolate, "modules"));
  args->Return(modules);
}

}  // namespace

DEFINE_WRAPPER_INFO(OSNative);

OSNative::OSNative(ScriptContext* context)
    : ThinNativeModule<OSNative>(context) {
}

OSNative::~OSNative() {
}

gin::ObjectTemplateBuilder OSNative::GetObjectTemplateBuilder(
    v8::Isolate* isolate) {
  return ThinNativeModule<OSNative>::GetObjectTemplateBuilder(isolate)
      .SetMethod("abort", AbortCallback)
      .SetMethod("chdir", ChangeDirectoryCallback)
      .SetMethod("getcwd", GetCurrentWorkingDirectoryCallback)
      .SetMethod("modules", ModulesCallback)
      .SetValue("version", GetSelfVersion())
      .SetValue("versions", GetThirdPartyVersions(isolate))
      .SetValue("features", GetFeatures(isolate))
      .SetValue("argv", GetArgv())
      .SetValue("execPath", GetExecutablePath())
      .SetValue("name", std::string("javascript"))
      .SetValue("platform", platform::SystemInfo::OperatingSystemCodeName())
      .SetValue("arch", platform::SystemInfo::PlatformArchitecture())
      .SetValue("is_posix", platform::SystemInfo::IsPosix())
      .SetValue("pid", base::GetCurrentProcId())
      .SetValue("environ", internal::CreateSystemEnv(isolate));
}

void OSNative::Invalidate() {
  ThinNativeModule<OSNative>::Invalidate();
}

}  // namespace zarun
