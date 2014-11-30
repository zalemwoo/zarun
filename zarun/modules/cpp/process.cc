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
#include "gin/converter.h"
#include "v8/include/v8.h"

#include "zarun/zarun_version.h"
#include "zarun/script_context.h"

#include "zarun/utils/path_util.h"

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
#define ARCH "x86_64"
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
  scoped_ptr<base::Environment> env(base::Environment::Create());

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
  scoped_ptr<base::Environment> env(base::Environment::Create());

  std::string env_value(*v8::String::Utf8Value(value));
  env->SetVar(prop.c_str(), env_value);

  // Whether it worked or not, always return rval.
  info.GetReturnValue().Set(value);
}

void EnvQuery(v8::Local<v8::String> property,
              const v8::PropertyCallbackInfo<v8::Integer>& info) {
  std::string prop = gin::V8ToString(property);
  scoped_ptr<base::Environment> env(base::Environment::Create());
  bool ret = env->HasVar(prop.c_str());
  if (ret)
    info.GetReturnValue().Set(0);
}

void EnvDeleter(v8::Local<v8::String> property,
                const v8::PropertyCallbackInfo<v8::Boolean>& info) {
  std::string prop = gin::V8ToString(property);
  scoped_ptr<base::Environment> env(base::Environment::Create());
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

}  // namespace

Process::Process(ScriptContext* context) : ObjectBackedNativeModule(context) {
  RouteFunction("cwd", base::Bind(&CwdCallback));
  RouteFunction("abort", base::Bind(&AbortCallback));
  RouteFunction("chdir", base::Bind(&ChdirCallback));
}

Process::~Process() {
}

v8::Handle<v8::Object> Process::NewInstance() {
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
  process->ForceSet(gin::StringToV8(isolate, "platform"),
                    gin::StringToV8(isolate, std::string(PLATFORM)),
                    v8::PropertyAttribute::ReadOnly);
  process->ForceSet(gin::StringToV8(isolate, "arch"),
                    gin::StringToV8(isolate, std::string(ARCH)),
                    v8::PropertyAttribute::ReadOnly);
  process->ForceSet(gin::StringToV8(isolate, "env"), GetEnvironment(isolate),
                    v8::PropertyAttribute::ReadOnly);
  process->ForceSet(gin::StringToV8(isolate, "pid"),
                    gin::ConvertToV8(isolate, base::GetCurrentProcId()),
                    v8::PropertyAttribute::ReadOnly);

  return scope.Escape(process);
}

}  // namespace zarun
