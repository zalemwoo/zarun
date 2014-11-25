#ifndef ZARUN_BACKEND_THREAD_H_
#define ZARUN_BACKEND_THREAD_H_

#include "base/callback.h"
#include "base/threading/thread.h"
#include "base/memory/weak_ptr.h"

#include "zarun/zarun_export.h"

namespace zarun {
namespace backend {

class BackendApplication;
class BackendThread;

typedef base::Callback<void(BackendThread*)> ThreadTerminateCallback;

class ZARUN_EXPORT BackendThread : public base::Thread {
 public:
  BackendThread(const std::string& name,
                const base::WeakPtr<BackendApplication>& application);

  ~BackendThread();

  void SetTerminateCallback(
      const ThreadTerminateCallback& termination_callback) {
    termination_callback_ = termination_callback;
  }

 protected:
  // Called just prior to starting the message loop
  virtual void Init() override;
  // Called just after the message loop ends
  virtual void CleanUp() override;

 private:
  base::Callback<void(BackendThread*)> termination_callback_;
  base::WeakPtr<BackendApplication> application_;

  DISALLOW_COPY_AND_ASSIGN(BackendThread);
};
}
}  // namespace zarun::backend

#endif  // ZARUN_BACKEND_THREAD_H_
