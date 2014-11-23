#include "zarun/backend/backend_thread.h"

#include "base/logging.h"
#include "base/bind.h"
#include "base/callback.h"
#include "base/message_loop/message_loop.h"

#include "gin/public/isolate_holder.h"
#include "gin/array_buffer.h"

namespace zarun {

namespace backend {

BackendThread::BackendThread(
    const std::string& name,
    const base::Callback<void(BackendThread*)>& termination_callback,
    zarun::ScriptRunnerDelegate* runner_delegate)
    : base::Thread(name),
      termination_callback_(termination_callback),
      runner_delegate_(runner_delegate) {
  CHECK(runner_delegate_.get());
}

BackendThread::~BackendThread() { base::Thread::Stop(); }

void BackendThread::Init() {
  gin::IsolateHolder::Initialize(gin::IsolateHolder::kStrictMode,
                                 gin::ArrayBufferAllocator::SharedInstance());

  isolate_holder_.reset(new gin::IsolateHolder());
  runner_.reset(new zarun::ScriptRunner(runner_delegate_.get(),
                                        isolate_holder_->isolate()));

  {
    gin::Runner::Scope scope(runner_.get());
    v8::V8::SetCaptureStackTraceForUncaughtExceptions(true);
  }
}

void BackendThread::CleanUp() {
  runner_delegate_.reset();
  runner_.reset();
  isolate_holder_.reset();
  termination_callback_.Run(this);
}
}
}  // namespace zarun::backend
