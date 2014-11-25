/*
 * backend_application.cc
 *
 *  Created on: Nov 18, 2014
 *      Author: zalem
 */

#include "zarun/backend/backend_application.h"
#include "zarun/backend/backend_runner_delegate.h"

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
    backend::BackendScriptRunnerDelegate* backend_script_runner_delegate)
    : backend_runner_delegate_(backend_script_runner_delegate),
      shell_runner_(shell_runner),
      termination_callback_(termination_callback),
      weak_factory_(this) {
  main_thread_.reset(new BackendThread("backend", this->GetWeakPtr()));
  main_thread_->SetTerminateCallback(
      base::Bind(&BackendApplication::OnThreadEnd, base::Unretained(this)));
}

BackendApplication::~BackendApplication() { backend_runner_delegate_.reset(); }

void BackendApplication::Start() {
  base::Thread::Options options;
  options.message_loop_type = base::MessageLoop::TYPE_IO;
  bool launched = main_thread_->StartWithOptions(options);
  DCHECK(launched);
  backend_runner_ = main_thread_->task_runner().get();
  CHECK(backend_runner_.get());
}

void BackendApplication::Stop() { main_thread_->Stop(); }

void BackendApplication::CreateEnvironment() {
  DCHECK(!environment_.get());
  environment_.reset(Environment::Create(backend_runner_delegate_.get()));
}

void BackendApplication::DisposeEnvironment() {
  DCHECK(environment_.get());
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
  shell_runner_->PostTask(FROM_HERE, base::Bind(termination_callback_, this));
}
}
}  // namespace zarun::backend
