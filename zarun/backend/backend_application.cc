/*
 * backend_application.cc
 *
 */

#include "zarun/backend/backend_application.h"

#include "base/bind.h"
#include "base/files/file_util.h"

namespace zarun {
namespace backend {

namespace {

std::string Load(const base::FilePath& path) {
  std::string source;
  if (!base::ReadFileToString(path, &source))
    LOG(FATAL) << "Unable to read " << path.LossyDisplayName();
  return source;
}

}  // namespace

BackendApplication::BackendApplication(
    scoped_refptr<base::TaskRunner> shell_runner,
    const base::Callback<void(BackendApplication*)>& termination_callback,
    const backend::RunScriptCallback& run_script_callback)
    : shell_runner_(shell_runner),
      termination_callback_(termination_callback),
      backend_runner_delegate_(
          CreateBackendScriptRunnerDelegate(run_script_callback)),
      weak_factory_(this) {
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

void BackendApplication::CreateEnvironment() {
  DCHECK(!environment_.get()) << "environment created already";
  environment_.reset(Environment::Create(backend_runner_delegate_.get()));
}

void BackendApplication::DisposeEnvironment() {
  environment_.reset();
}

void BackendApplication::RunScript(const base::FilePath& path) {
  this->RunScript(Load(path), path.AsUTF8Unsafe());
}

void BackendApplication::RunScript(const std::string& source,
                                   const std::string& resource_name) {
  backend_runner_->PostTask(
      FROM_HERE, base::Bind(&BackendApplication::run_script_,
                            base::Unretained(this), source, resource_name));
}

void BackendApplication::run_script_(const std::string& source,
                                     const std::string& resource_name) {
  zarun::ScriptRunner* script_runner = environment_->runner();
  gin::Runner::Scope scope(script_runner);
  script_runner->Run(source, resource_name);
}

// must be sync called on backend thread.
void BackendApplication::OnThreadEnd(BackendThread* thread) {
}
}
}  // namespace zarun::backend
