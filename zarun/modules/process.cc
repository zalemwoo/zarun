/*
 * process.cc
 *
 *  Created on: Nov 20, 2014
 *      Author: zalem
 */

// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "zarun/modules/process.h"

#include <set>
#include <vector>
#include <string>

#include "base/sys_info.h"
#include "base/environment.h"
#include "base/strings/string_util.h"
#include "gin/arguments.h"
#include "gin/converter.h"
#include "gin/object_template_builder.h"
#include "gin/per_isolate_data.h"
#include "gin/per_context_data.h"
#include "gin/public/wrapper_info.h"
#include "v8/include/v8.h"

#ifdef OS_POSIX
#include <unistd.h>
#endif

#include "zarun/zarun_version.h"
#include "zarun/modules/module_registry.h"

namespace zarun {

namespace {

// Key for base::SupportsUserData::Data.
const char kSystemEnvironmentKey[] = "SystemEnvironment";

struct SystemEnvironment : public base::SupportsUserData::Data {
  scoped_ptr<base::Environment> environment;
};

v8::Handle<v8::Value> VersionCallback(gin::Arguments* args) {
  return gin::Converter<std::string>::ToV8(args->isolate(),
                                           std::string(ZARUN_VERSION_FULL));
}

v8::Handle<v8::Object> SetVersions(v8::Isolate* isolate) {
  v8::Local<v8::ObjectTemplate> versions_templ =
      gin::ObjectTemplateBuilder(isolate)
          .SetValue("v8", std::string(v8::V8::GetVersion()))
          .Build();

  return versions_templ->NewInstance();
}

std::vector<std::string> ModuleListCallback(gin::Arguments* args) {
  v8::Local<v8::Context> context = args->isolate()->GetCurrentContext();
  ModuleRegistry* module_registry = ModuleRegistry::From(context);
  std::set<std::string> modules = module_registry->available_modules();
  return std::vector<std::string>(modules.begin(), modules.end());
}

// Environment
// static
base::Environment* GetSystemEnvironment(v8::Handle<v8::Context> context) {
  gin::PerContextData* per_context_data = gin::PerContextData::From(context);
  if (!per_context_data) return NULL;

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
    info.GetReturnValue().Set(gin::StringToSymbol(isolate, env_value));
    return;
  }
  // Not found.  Fetch from prototype.
  info.GetReturnValue().Set(info.Data().As<v8::Object>()->Get(property));
}

void EnvSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value,
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
  if (ret) info.GetReturnValue().Set(0);
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
#ifdef OS_POSIX
  int size = 0;
  while (environ[size]) size++;

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
  if (environment == NULL) return;  // This should not happen.
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

v8::Handle<v8::Object> SetEnvironment(v8::Isolate* isolate) {
  v8::Local<v8::ObjectTemplate> env_templ = v8::ObjectTemplate::New();

  env_templ->SetNamedPropertyHandler(EnvGetter, EnvSetter, EnvQuery, EnvDeleter,
                                     EnvEnumerator, v8::Object::New(isolate));
  return env_templ->NewInstance();
}
gin::WrapperInfo g_wrapper_info = {gin::kEmbedderNativeGin};

}  // namespace

const char Process::kModuleName[] = "process";

v8::Local<v8::Value> Process::GetModule(v8::Isolate* isolate) {
  gin::PerIsolateData* data = gin::PerIsolateData::From(isolate);
  v8::Local<v8::ObjectTemplate> templ =
      data->GetObjectTemplate(&g_wrapper_info);
  if (templ.IsEmpty()) {
    templ = gin::ObjectTemplateBuilder(isolate)
                .SetProperty("version", &VersionCallback)
                .SetValue("versions", SetVersions(isolate))
                .SetValue("platform", base::StringToLowerASCII(
                                          base::SysInfo::OperatingSystemName()))
                .SetValue("arch", base::SysInfo::OperatingSystemArchitecture())
                .SetProperty("moduleLoadList", &ModuleListCallback)
                .SetValue("env", SetEnvironment(isolate))
                .Build();

    data->SetObjectTemplate(&g_wrapper_info, templ);
  }
  return templ->NewInstance();
}

}  // namespace zarun
