#include "zarun/backend/backend_thread.h"
#include "zarun/backend/backend_application.h"

#include "base/bind.h"

namespace zarun {

namespace backend {

BackendThread::BackendThread(
    const std::string& name,
    const base::WeakPtr<BackendApplication>& application)
    : base::Thread(name), termination_callback_(), application_(application) {
}

BackendThread::~BackendThread() {
  base::Thread::Stop();
}

void BackendThread::Init() {
  if (application_.get()) {
    application_.get()->CreateEnvironment();
  }
}

void BackendThread::CleanUp() {
  if (application_.get()) {
    application_.get()->DisposeEnvironment();
  }
  application_.reset();

  if (!termination_callback_.is_null()) {
    termination_callback_.Run(this);
  }
}
}
}  // namespace zarun::backend
