#ifndef ZARUN_BACKEND_THREAD_H_
#define ZARUN_BACKEND_THREAD_H_

#include "base/threading/thread.h"
#include "base/memory/scoped_ptr.h"

#include "zarun/zarun_export.h"
#include "zarun/script_runner.h"

namespace gin {
class IsolateHolder;
}

namespace zarun {

namespace backend {

class ZARUN_EXPORT BackendThread : public base::Thread {
 public:
  BackendThread(
      const std::string& name,
      const base::Callback<void(BackendThread*)>& termination_callback,
      zarun::ScriptRunnerDelegate* runnerDelegate);

  ~BackendThread();

  zarun::ScriptRunner* shell_runner() const { return runner_.get(); }

 protected:
  // Called just prior to starting the message loop
  virtual void Init() override;

  // Called just after the message loop ends
  virtual void CleanUp() override;

 private:
  base::Callback<void(BackendThread*)> termination_callback_;
  scoped_ptr<gin::IsolateHolder> isolateHolder_;
  scoped_ptr<zarun::ScriptRunner> runner_;
  scoped_ptr<zarun::ScriptRunnerDelegate> runnerDelegate_;

  DISALLOW_COPY_AND_ASSIGN(BackendThread);
};
}
}  // namespace zarun::backend

#endif  // ZARUN_BACKEND_THREAD_H_
