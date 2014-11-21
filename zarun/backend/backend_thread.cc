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
    zarun::ScriptRunnerDelegate* runnerDelegate)
    : base::Thread(name),
      termination_callback_(termination_callback),
      runnerDelegate_(runnerDelegate) {
  CHECK(runnerDelegate_.get());
}

BackendThread::~BackendThread() { base::Thread::Stop(); }

void BackendThread::Init() {
  gin::IsolateHolder::Initialize(gin::IsolateHolder::kStrictMode,
                                 gin::ArrayBufferAllocator::SharedInstance());

  isolateHolder_.reset(new gin::IsolateHolder());
  runner_.reset(new zarun::ScriptRunner(runnerDelegate_.get(),
                                        isolateHolder_->isolate()));

  {
    gin::Runner::Scope scope(runner_.get());
    v8::V8::SetCaptureStackTraceForUncaughtExceptions(true);
  }
}

void BackendThread::CleanUp() {
  runnerDelegate_.reset();
  runner_.reset();
  isolateHolder_.reset();
  termination_callback_.Run(this);
}
}
}  // namespace zarun::backend
