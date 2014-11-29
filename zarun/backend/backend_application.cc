/*
 * backend_application.cc
 *
 */

#include "zarun/backend/backend_application.h"

#include "base/bind.h"
#include "gin/array_buffer.h"

#include "zarun/zarun_shell.h"  // for GetDefaultV8Options()
#include "zarun/utils/file_util.h"

namespace zarun {
namespace backend {

namespace {

bool v8_inited = false;

}  // namespace

BackendApplication::BackendApplication(
    scoped_refptr<base::TaskRunner> shell_runner,
    const ApplicationTerminateCallback& termination_callback,
    scoped_ptr<BackendScriptContextDelegate> script_runner_delegate)
    : shell_runner_(shell_runner),
      termination_callback_(termination_callback),
      backend_context_delegate_(script_runner_delegate.Pass()),
      weak_factory_(this) {
}

BackendApplication::BackendApplication(
    scoped_refptr<base::TaskRunner> shell_runner,
    const base::Callback<void(BackendApplication*)>& termination_callback,
    const backend::RunScriptCallback& run_script_callback)
    : shell_runner_(shell_runner),
      termination_callback_(termination_callback),
      backend_context_delegate_(
          CreateBackendScriptContextDelegate(run_script_callback)),
      weak_factory_(this) {
  if (!v8_inited) {
    v8_inited = true;
    gin::IsolateHolder::Initialize(gin::IsolateHolder::kStrictMode,
                                   gin::ArrayBufferAllocator::SharedInstance());
    const std::string& v8options = ::zarun::GetDefaultV8Options();
    v8::V8::SetFlagsFromString(v8options.c_str(), v8options.length());
  }
}

BackendApplication::~BackendApplication() {
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
  environment_.reset(new Environment(isolate, backend_context_delegate_.get()));
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
