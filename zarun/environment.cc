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
Environment* Environment::Create(
    zarun::ScriptContextDelegate* script_context_delegate) {
  if (!v8_inited) {
    v8_inited = true;
    gin::IsolateHolder::Initialize(gin::IsolateHolder::kStrictMode,
                                   gin::ArrayBufferAllocator::SharedInstance());
    const std::string& v8options = ::zarun::GetDefaultV8Options();
    v8::V8::SetFlagsFromString(v8options.c_str(), v8options.length());
  }

  DCHECK(script_context_delegate);
  return new Environment(script_context_delegate);
}

Environment::Environment(zarun::ScriptContextDelegate* script_context_delegate)
    : isolate_holder_() {
  script_context_.reset(
      new ScriptContext(script_context_delegate, isolate_holder_.isolate()));
  {
    gin::Runner::Scope scope(script_context_.get());
    v8::V8::SetCaptureStackTraceForUncaughtExceptions(true);
  }
}

Environment::~Environment() {
  script_context_.reset();

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

v8::Isolate* Environment::isolate() {
  return this->isolate_holder_.isolate();
}

v8::Local<v8::Context> Environment::v8_context() {
  return this->script_context_->v8_context();
}

}  // namespace zarun
