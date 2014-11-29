#include "zarun/backend/backend_thread.h"
#include "zarun/backend/backend_application.h"

#include "base/bind.h"
#include "gin/public/isolate_holder.h"

namespace zarun {

namespace backend {

BackendThread::BackendThread(
    const std::string& name,
    const base::WeakPtr<BackendApplication>& application)
    : base::Thread(name), application_(application), termination_callback_() {
}

BackendThread::~BackendThread() {
  base::Thread::Stop();
}

void BackendThread::Init() {
  base::Thread::Init();
  isolate_holder_.reset(new gin::IsolateHolder());
  v8::Isolate* isolate = isolate_holder_->isolate();
  v8::Isolate::Scope isolate_scope(isolate);
  v8::V8::SetCaptureStackTraceForUncaughtExceptions(true);
  if (application_.get()) {
    application_.get()->CreateEnvironment(isolate);
  }
}

void BackendThread::CleanUp() {
  if (application_.get()) {
    application_.get()->DisposeEnvironment();
  }
  application_.reset();

  // do GC for clean heap.
  v8::Isolate* isolate = isolate_holder_->isolate();
  v8::HeapStatistics stats;
  isolate->GetHeapStatistics(&stats);
  size_t old_heap_size = 0;
  // Run the GC until the heap size reaches a steady state to ensure that
  // all the garbage is collected.
  while (stats.used_heap_size() != old_heap_size) {
    old_heap_size = stats.used_heap_size();
    isolate->RequestGarbageCollectionForTesting(
        v8::Isolate::kFullGarbageCollection);
    isolate->GetHeapStatistics(&stats);
  }

  isolate_holder_.reset();

  if (!termination_callback_.is_null()) {
    termination_callback_.Run(this);
  }

  base::Thread::CleanUp();
}
}
}  // namespace zarun::backend
