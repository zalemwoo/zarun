#ifndef ZARUN_BACKEND_THREAD_H_
#define ZARUN_BACKEND_THREAD_H_

#include "base/callback.h"
#include "base/threading/thread.h"
#include "base/memory/weak_ptr.h"

#include "zarun/zarun_export.h"

namespace gin {
class IsolateHolder;
}

namespace zarun {
namespace backend {

class BackendApplication;

class ZARUN_EXPORT BackendThread : public base::Thread {
 public:
  typedef base::Callback<void(BackendThread*)> ThreadTerminateCallback;
  ~BackendThread();

  void SetTerminateCallback(
      const ThreadTerminateCallback& termination_callback) {
    termination_callback_ = termination_callback;
  }

 protected:
  BackendThread(const std::string& name,
                const base::WeakPtr<BackendApplication>& application);

  // Called just prior to starting the message loop
  virtual void Init() override;
  // Called just after the message loop ends
  virtual void CleanUp() override;

 private:
  base::WeakPtr<BackendApplication> application_;
  ThreadTerminateCallback termination_callback_;
  scoped_ptr<gin::IsolateHolder> isolate_holder_;
  friend BackendApplication;

  DISALLOW_COPY_AND_ASSIGN(BackendThread);
};
}
}  // namespace zarun::backend

#endif  // ZARUN_BACKEND_THREAD_H_
