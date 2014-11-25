/*
 * environment.h
 *
 */

#ifndef ZARUN_ENVIRONMENT_H_
#define ZARUN_ENVIRONMENT_H_

#include "zarun/zarun_export.h"
#include "zarun/script_runner.h"

#include "gin/public/isolate_holder.h"
#include "v8/include/v8.h"

namespace zarun {

class ZARUN_EXPORT Environment {
 public:
  static Environment* Create(zarun::ScriptRunnerDelegate* runner_delegate);

  ~Environment();

  v8::Isolate* isolate();
  v8::Local<v8::Context> context();

  zarun::ScriptRunner* runner() { return runner_.get(); }

 private:
  Environment(zarun::ScriptRunnerDelegate* runner_delegate);
  gin::IsolateHolder isolate_holder_;
  scoped_ptr<zarun::ScriptRunner> runner_;
};

}  // namespace zarun

#endif  // ZARUN_ENVIRONMENT_H_
