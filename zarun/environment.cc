/*
 * environment.cc
 *
 */

#include "zarun/environment.h"

#include "gin/array_buffer.h"

#include "zarun/zarun_shell.h"  // for GetDefaultV8Options()

namespace zarun {

namespace {
bool v8_inited = false;
}  // namespace

// static
Environment* Environment::Create(zarun::ScriptRunnerDelegate* runner_delegate) {
  if (!v8_inited) {
    v8_inited = true;
    gin::IsolateHolder::Initialize(gin::IsolateHolder::kStrictMode,
                                   gin::ArrayBufferAllocator::SharedInstance());
    const std::string& v8options = ::zarun::GetDefaultV8Options();
    v8::V8::SetFlagsFromString(v8options.c_str(), v8options.length());
  }

  DCHECK(runner_delegate);
  return new Environment(runner_delegate);
}

Environment::Environment(zarun::ScriptRunnerDelegate* runner_delegate)
    : isolate_holder_() {
  runner_.reset(new ScriptRunner(runner_delegate, isolate_holder_.isolate()));
  {
    gin::Runner::Scope scope(runner_.get());
    v8::V8::SetCaptureStackTraceForUncaughtExceptions(true);
  }
}

Environment::~Environment() {
  runner_.reset();

  v8::HeapStatistics stats;
  v8::Isolate* isolate = isolate_holder_.isolate();
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
}

v8::Isolate* Environment::isolate() { return this->isolate_holder_.isolate(); }

v8::Local<v8::Context> Environment::context() {
  return this->runner_->GetContextHolder()->context();
}

}  // namespace zarun
