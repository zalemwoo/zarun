/*
 * backend_application.cc
 *
 */

#include "zarun/backend/backend_application.h"

#include "base/bind.h"
#include "base/logging.h"
#include "gin/array_buffer.h"
#include "v8/include/v8.h"

#include "zarun/zarun_shell.h"  // for GetDefaultV8Options()
#include "zarun/utils/file_util.h"
#include "zarun/modules/javascript_module_system.h"
#include "zarun/modules/cpp/process.h"

namespace zarun {
namespace backend {

namespace {

static bool v8_inited = false;

void InitiateV8() {
  if (v8_inited)
    return;

  v8_inited = true;
  gin::IsolateHolder::Initialize(gin::IsolateHolder::kStrictMode,
                                 gin::ArrayBufferAllocator::SharedInstance());
  const std::string& v8options = ::zarun::GetDefaultV8Options();
  v8::V8::SetFlagsFromString(v8options.c_str(), v8options.length());
}

void DisposeV8() {
  v8::V8::Dispose();
  v8::V8::ShutdownPlatform();
}

void DidCreateEnvironmentCallback(zarun::Environment* env) {
  v8::Isolate* isolate = env->isolate();
  v8::HandleScope scope(isolate);
  JavaScriptModuleSystem* module_system = env->context()->module_system();
  module_system->RegisterNativeModule(
      "process", scoped_ptr<NativeJavaScriptModule>(
                     new ProcessNative(env->context())).Pass());
  // needed for enable requireNative() call from javascript.
  JavaScriptModuleSystem::NativesEnabledScope natives_scope(module_system);
  module_system->Require("bootstrap");
}

}  // namespace

BackendApplication::BackendApplication(
    scoped_refptr<base::TaskRunner> shell_runner,
    const ApplicationTerminateCallback& termination_callback,
    scoped_ptr<BackendScriptContextDelegate> script_runner_delegate)
    : shell_runner_(shell_runner),
      termination_callback_(termination_callback),
      backend_context_delegate_(script_runner_delegate.Pass()),
      weak_factory_(this) {
  InitiateV8();
}

BackendApplication::BackendApplication(
    scoped_refptr<base::TaskRunner> shell_runner,
    const base::Callback<void(BackendApplication*)>& termination_callback,
    const backend::RunScriptCallback& run_script_callback)
    : BackendApplication(
          shell_runner,
          termination_callback,
          CreateBackendScriptContextDelegate(run_script_callback)) {
}

BackendApplication::~BackendApplication() {
  DisposeV8();
}

void BackendApplication::Start() {
  base::Thread::Options options;
  options.message_loop_type = base::MessageLoop::TYPE_IO;

  main_thread_.reset(new BackendThread("backend", this->GetWeakPtr()));
  main_thread_->SetTerminateCallback(
      base::Bind(&BackendApplication::OnThreadEnd, base::Unretained(this)));

  bool launched = main_thread_->StartWithOptions(options);
  DCHECK(launched);
  backend_runner_ = main_thread_->task_runner().get();
  CHECK(backend_runner_.get());
}

void BackendApplication::Stop() {
  main_thread_->Stop();
  main_thread_.reset();
  shell_runner_->PostTask(FROM_HERE, base::Bind(termination_callback_, this));
}

void BackendApplication::CreateEnvironment(v8::Isolate* isolate) {
  DCHECK(!environment_.get()) << "environment created already";
  environment_.reset(
      new Environment(isolate, backend_context_delegate_.get(),
                      base::Bind(&DidCreateEnvironmentCallback)));
}

void BackendApplication::DisposeEnvironment() {
  environment_.reset();
}

void BackendApplication::RunScript(const base::FilePath& path) {
  this->RunScript(zarun::util::ReadFile(path), path.AsUTF8Unsafe());
}

void BackendApplication::RunScript(const std::string& source,
                                   const std::string& resource_name) {
  backend_runner_->PostTask(
      FROM_HERE, base::Bind(&BackendApplication::run_script_,
                            base::Unretained(this), source, resource_name));
}

void BackendApplication::run_script_(const std::string& source,
                                     const std::string& resource_name) {
  zarun::ScriptContext* script_context = environment_->context();
  gin::Runner::Scope scope(script_context);
  script_context->Run(source, resource_name);
}

// must be sync called on backend thread.
void BackendApplication::OnThreadEnd(BackendThread* thread) {
}
}
}  // namespace zarun::backend
