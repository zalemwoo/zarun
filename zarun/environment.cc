/*
 * environment.cc
 *
 */

#include "zarun/environment.h"

#include "base/lazy_instance.h"
#include "base/stl_util.h"

#include "gin/public/context_holder.h"
#include "gin/per_context_data.h"

#include "zarun/utils/file_util.h"
#include "zarun/utils/path_util.h"
#include "zarun/safe_builtins.h"
#include "zarun/modules/native_source_map.h"
#include "zarun/modules/common_module_system.h"

namespace zarun {

namespace {

// Key for base::SupportsUserData::Data.
const char kEnvironmentKey[] = "ZarunEnvironment";

struct EnvironmentData : public base::SupportsUserData::Data {
  Environment* env;
};

class V8ExtensionConfigurator {
 public:
  V8ExtensionConfigurator()
      : safe_builtins_(SafeBuiltins::CreateV8Extension()),
        names_(1, safe_builtins_->name()),
        configuration_(
            new v8::ExtensionConfiguration(static_cast<int>(names_.size()),
                                           vector_as_array(&names_))) {
    v8::RegisterExtension(safe_builtins_.get());
  }

  v8::ExtensionConfiguration* GetConfiguration() {
    return configuration_.get();
  }

 private:
  scoped_ptr<v8::Extension> safe_builtins_;
  std::vector<const char*> names_;
  scoped_ptr<v8::ExtensionConfiguration> configuration_;
};

base::LazyInstance<V8ExtensionConfigurator>::Leaky g_v8_extension_configurator =
    LAZY_INSTANCE_INITIALIZER;

}  // namespace

Environment::Environment(v8::Isolate* isolate,
                         zarun::ScriptContextDelegate* script_context_delegate,
                         const EnvironmentCreatedCallback& created_callback)
    : isolate_(isolate),
      context_holder_(new gin::ContextHolder(isolate)),
      handle_scope_(isolate_),
      source_map_(new NativeSourceMap()) {
  v8::Isolate::Scope isolate_scope(isolate);
  v8::HandleScope handle_scope(isolate);
  v8::Handle<v8::Context> v8_context = v8::Context::New(
      isolate, g_v8_extension_configurator.Get().GetConfiguration(),
      script_context_delegate->GetGlobalTemplate(isolate));

  context_holder_->SetContext(v8_context);

  // register environment to its context, so later can use static method
  // Environment::From(v8_context) to get the per context Environment.
  gin::PerContextData* data = gin::PerContextData::From(v8_context);
  EnvironmentData* env_data = new EnvironmentData();
  env_data->env = this;
  data->SetUserData(kEnvironmentKey, env_data);

  script_context_.reset(new ScriptContext(script_context_delegate, v8_context));
  script_context_->v8_context()->Enter();

  script_context_->set_module_system(
      CommonModuleSystem::Create(script_context_.get(), source_map_.get()));

  created_callback.Run(this);
}

Environment::~Environment() {
  {
    v8::Isolate::Scope isolate_scope(isolate_);
    v8::HandleScope handle_scope(isolate_);
    if (script_context_)
      script_context_->v8_context()->Exit();

    script_context_.reset();
    context_holder_.reset();
  }
}

// static
Environment* Environment::From(v8::Handle<v8::Context> context) {
  gin::PerContextData* data = gin::PerContextData::From(context);
  if (!data)
    return NULL;

  EnvironmentData* env_data =
      static_cast<EnvironmentData*>(data->GetUserData(kEnvironmentKey));
  return env_data->env;
}

v8::Isolate* Environment::isolate() {
  return isolate_;
}

v8::Local<v8::Context> Environment::v8_context() {
  return this->script_context_->v8_context();
}

void Environment::RegisterModuleFileForTest(const std::string& name,
                                            const base::FilePath& relate_path) {
  base::FilePath full_path =
      zarun::util::MakeAbsoluteFilePathRelateToProgram(relate_path);
  RegisterModule(name, zarun::util::ReadFile(full_path));
}

void Environment::RegisterModule(const std::string& name,
                                 const std::string& code) {
  source_map_->RegisterModule(name, code);
}

}  // namespace zarun
