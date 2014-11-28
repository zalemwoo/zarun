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
#include "base/sys_info.h"
#include "base/environment.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "base/process/process.h"
#include "base/process/process_handle.h"
#include "base/process/kill.h"
#include "base/files/file_util.h"
#include "gin/arguments.h"
#include "gin/converter.h"
#include "gin/object_template_builder.h"
#include "gin/per_isolate_data.h"
#include "gin/per_context_data.h"
#include "gin/public/wrapper_info.h"
#include "v8/include/v8.h"

#include "zarun/zarun_version.h"
#include "zarun/modules/module_registry.h"

#if defined(OS_POSIX)
#include <unistd.h>
#if defined(OS_MACOSX)
#include <crt_externs.h>
#define environ (*_NSGetEnviron())
#elif !defined(COMPILER_MSVC)
extern char** environ;
#endif
#endif

namespace zarun {

namespace {

// Key for base::SupportsUserData::Data.
const char kSystemEnvironmentKey[] = "SystemEnvironment";

struct SystemEnvironment : public base::SupportsUserData::Data {
  scoped_ptr<base::Environment> environment;
};

#if defined(OS_MACOSX)
#define PLATFORM "darwin"
#elif defined(OS_LINUX)
#define PLATFORM "linux"
#elif defined(OS_FREEBSD)
#define PLATFORM "freebsd"
#elif defined(OS_ANDROID)
#define PLATFORM "android"
#elif defined(OS_WIN)
#define PLATFORM "windows"
#else
#error unknown platform
#endif

#if defined(ARCH_CPU_X86_FAMILY)
#if defined(ARCH_CPU_X86_64)
#define ARCH "x64"
#elif defined(ARCH_CPU_X86)
#define ARCH "x86"
#else
#error unknown arch
#endif
#elif defined(ARCH_CPU_ARM_FAMILY)
#define ARCH "arm"
#else
#error unknown arch
#endif

// process.version
std::string GetVersion() {
  return std::string(ZARUN_VERSION_FULL);
}

// process.versions
v8::Handle<v8::Object> GetVersions(v8::Isolate* isolate) {
  v8::Local<v8::Object> versions_obj = v8::Object::New(isolate);
  versions_obj->ForceSet(
      gin::StringToV8(isolate, "v8"),
      gin::StringToV8(isolate, std::string(v8::V8::GetVersion())),
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
  base::FilePath executablePath =
      base::CommandLine::ForCurrentProcess()->GetProgram();
  return base::MakeAbsoluteFilePath(executablePath).AsUTF8Unsafe();
}

// process.moduleLoadList
std::vector<std::string> ModuleListCallback(gin::Arguments* args) {
  v8::Local<v8::Context> context = args->isolate()->GetCurrentContext();
  ModuleRegistry* module_registry = ModuleRegistry::From(context);
  std::set<std::string> modules = module_registry->available_modules();
  return std::vector<std::string>(modules.begin(), modules.end());
}

// process.pid
base::ProcessId ProcessIdCallback() {
  return base::GetCurrentProcId();
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

// process.reallyExit
void ExitCallback(gin::Arguments* args) {
  int exit_code = 0;
  args->GetNext(&exit_code);
  fclose(stdout);
  fclose(stderr);
  exit(exit_code);
}

// process.kill
int KillCallback(gin::Arguments* args) {
  int pid;
  if (args->Length() != 2 || !args->GetNext(&pid)) {
    args->ThrowTypeError("Bad argument.");
    return -1;
  }
  if (!base::KillProcess(pid, 0, false)) {
    return -::logging::GetLastSystemErrorCode();
  }
  return 0;
}

// process.abort
void AbortCallback() {
  abort();
}

// process.chdir
void ChdirCallback(gin::Arguments* args) {
  std::string path;
  if (args->Length() != 1 || !args->GetNext(&path)) {
    args->ThrowTypeError("Bad argument.");
    return;
  }

  if (!base::SetCurrentDirectory(base::FilePath(path))) {
    args->ThrowError();
  }
}

// process.cwd
std::string CwdCallback(gin::Arguments* args) {
  base::FilePath cwd;
  if (!base::GetCurrentDirectory(&cwd)) {
    args->ThrowError();
    return std::string();
  }
  return cwd.AsUTF8Unsafe();
}

// process.env
// static
base::Environment* GetSystemEnvironment(v8::Handle<v8::Context> context) {
  gin::PerContextData* per_context_data = gin::PerContextData::From(context);
  if (!per_context_data)
    return NULL;

  SystemEnvironment* env_data = static_cast<SystemEnvironment*>(
      per_context_data->GetUserData(kSystemEnvironmentKey));

  if (!env_data) {
    // PerContextData takes ownership of ModuleRegistryData.
    env_data = new SystemEnvironment();
    env_data->environment.reset(base::Environment::Create());
    per_context_data->SetUserData(kSystemEnvironmentKey, env_data);
  }
  return env_data->environment.get();
}

void EnvGetter(v8::Local<v8::String> property,
               const v8::PropertyCallbackInfo<v8::Value>& info) {
  std::string prop = gin::V8ToString(property);
  v8::Isolate* isolate = info.GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  base::Environment* env = GetSystemEnvironment(context);

  std::string env_value;
  bool ret = env->GetVar(prop.c_str(), &env_value);

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
  v8::Isolate* isolate = info.GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  base::Environment* env = GetSystemEnvironment(context);

  std::string env_value(*v8::String::Utf8Value(value));
  env->SetVar(prop.c_str(), env_value);

  // Whether it worked or not, always return rval.
  info.GetReturnValue().Set(value);
}

void EnvQuery(v8::Local<v8::String> property,
              const v8::PropertyCallbackInfo<v8::Integer>& info) {
  std::string prop = gin::V8ToString(property);
  v8::Isolate* isolate = info.GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  base::Environment* env = GetSystemEnvironment(context);
  bool ret = env->HasVar(prop.c_str());
  if (ret)
    info.GetReturnValue().Set(0);
}

void EnvDeleter(v8::Local<v8::String> property,
                const v8::PropertyCallbackInfo<v8::Boolean>& info) {
  std::string prop = gin::V8ToString(property);
  v8::Isolate* isolate = info.GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  base::Environment* env = GetSystemEnvironment(context);
  bool ret = env->UnSetVar(prop.c_str());
  info.GetReturnValue().Set(ret);
}

void EnvEnumerator(const v8::PropertyCallbackInfo<v8::Array>& info) {
  v8::Isolate* isolate = info.GetIsolate();
#if defined(OS_POSIX)
  int size = 0;
  while (environ[size])
    size++;

  v8::Local<v8::Array> envarr = v8::Array::New(isolate, size);

  for (int i = 0; i < size; ++i) {
    const char* var = environ[i];
    const char* s = strchr(var, '=');
    const int length = s ? s - var : strlen(var);
    v8::Local<v8::String> name = v8::String::NewFromUtf8(
        isolate, var, v8::String::kNormalString, length);
    envarr->Set(i, name);
  }
#else  // _WIN32
  WCHAR* environment = GetEnvironmentStringsW();
  if (environment == NULL)
    return;  // This should not happen.
  v8::Local<v8::Array> envarr = v8::Array::New(isolate);
  WCHAR* p = environment;
  int i = 0;
  while (*p != NULL) {
    WCHAR* s;
    if (*p == L'=') {
      // If the key starts with '=' it is a hidden environment variable.
      p += wcslen(p) + 1;
      continue;
    } else {
      s = wcschr(p, L'=');
    }
    if (!s) {
      s = p + wcslen(p);
    }
    const uint16_t* two_byte_buffer = reinterpret_cast<const uint16_t*>(p);
    const size_t two_byte_buffer_len = s - p;
    v8::Local<v8::String> value = v8::String::NewFromTwoByte(
        isolate, two_byte_buffer, v8::String::kNormalString,
        two_byte_buffer_len);
    envarr->Set(i++, value);
    p = s + wcslen(s) + 1;
  }
  FreeEnvironmentStringsW(environment);
#endif

  info.GetReturnValue().Set(envarr);
}

v8::Handle<v8::Object> GetEnvironment(v8::Isolate* isolate) {
  v8::Local<v8::ObjectTemplate> env_templ = v8::ObjectTemplate::New();

  env_templ->SetNamedPropertyHandler(EnvGetter, EnvSetter, EnvQuery, EnvDeleter,
                                     EnvEnumerator, v8::Object::New(isolate));
  return env_templ->NewInstance();
}

void ModuleLoaded(gin::Arguments* args,
                  std::string module_name,
                  v8::Handle<v8::Value> module) {
  v8::Isolate* isolate = args->isolate();
  if (module.IsEmpty()) {
    isolate->ThrowException(v8::Exception::ReferenceError(gin::StringToV8(
        isolate,
        base::StringPrintf("No such module: %s.", module_name.c_str()))));
    return;
  }
  return args->Return(module);
}

// process.binding
void BindingCallback(gin::Arguments* args) {
  std::string module_name;
  if (args->Length() != 1 || !args->GetNext(&module_name)) {
    return args->ThrowTypeError("Bad argument.");
  }

  v8::Isolate* isolate = args->isolate();
  v8::HandleScope scope(isolate);
  v8::Handle<v8::Value> exports;
  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  ModuleRegistry* module_registry = ModuleRegistry::From(context);

    module_registry->LoadModule(isolate, module_name,
                                base::Bind(&ModuleLoaded, args, module_name));
}

gin::WrapperInfo g_wrapper_info = {gin::kEmbedderNativeGin};

}  // namespace

const char Process::kModuleName[] = "process";

v8::Local<v8::Value> Process::GetModule(v8::Isolate* isolate) {
  gin::PerIsolateData* data = gin::PerIsolateData::From(isolate);
  v8::Local<v8::ObjectTemplate> templ =
      data->GetObjectTemplate(&g_wrapper_info);
  if (templ.IsEmpty()) {
    v8::Handle<v8::Value> eventsObject = v8::Object::New(isolate);
    templ = gin::ObjectTemplateBuilder(isolate)
                .SetValue("versions", GetVersions(isolate))
                .SetValue("argv", GetArgv())
                .SetValue("execPath", GetExecutablePath())
                .SetValue("platform", std::string(PLATFORM))
                .SetValue("arch", std::string(ARCH))
                .SetProperty("moduleLoadList", &ModuleListCallback)
                .SetValue("env", GetEnvironment(isolate))
                .SetProperty("pid", &ProcessIdCallback)
                .SetValue("features", GetFeatures(isolate))
                .SetMethod("reallyExit", &ExitCallback)
                .SetMethod("abort", &AbortCallback)
                .SetMethod("_kill", &KillCallback)
                .SetMethod("chdir", &ChdirCallback)
                .SetMethod("cwd", &CwdCallback)
                .SetMethod("binding", &BindingCallback)
                .SetValue("_events", eventsObject)
                .Build();

    templ->Set(
        gin::StringToV8(isolate, "version"),
        gin::StringToV8(isolate, GetVersion()),
        static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

    data->SetObjectTemplate(&g_wrapper_info, templ);
  }
  return templ->NewInstance();
}

}  // namespace zarun
