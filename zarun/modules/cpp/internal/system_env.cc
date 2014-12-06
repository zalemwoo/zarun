/*
 * env.cc
 *
 */

#include "zarun/modules/cpp/internal/system_env.h"

#include "gin/converter.h"
#include "platform/base/environment.h"

namespace zarun {
namespace internal {

namespace {

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
    info.GetReturnValue().Set(0u);
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

}  // namespace

v8::Handle<v8::Object> CreateSystemEnv(v8::Isolate* isolate) {
  v8::Local<v8::ObjectTemplate> env_templ = v8::ObjectTemplate::New();
  env_templ->SetNamedPropertyHandler(EnvGetter, EnvSetter, EnvQuery, EnvDeleter,
                                     EnvEnumerator, v8::Object::New(isolate));
  return env_templ->NewInstance();
}
}
}  // namespace zarun::internal
