/*
 * backend_application.h
 *
 */

#ifndef ZARUN_BACKEND_BACKEND_APPLICATION_H_
#define ZARUN_BACKEND_BACKEND_APPLICATION_H_

#include "base/callback.h"
#include "base/task_runner.h"
#include "base/files/file_path.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"

#include "zarun/zarun_export.h"
#include "zarun/environment.h"
#include "zarun/backend/backend_thread.h"
#include "zarun/backend/backend_context_delegate.h"

namespace zarun {
namespace backend {

class ZARUN_EXPORT BackendApplication {
 public:
  typedef base::Callback<void(BackendApplication*)>
      ApplicationTerminateCallback;

  BackendApplication(
      scoped_refptr<base::TaskRunner> shell_runner,
      const ApplicationTerminateCallback& termination_callback,
      scoped_ptr<backend::BackendScriptContextDelegate> script_runner_delegate);

  BackendApplication(scoped_refptr<base::TaskRunner> shell_runner,
                     const ApplicationTerminateCallback& termination_callback,
                     const backend::RunScriptCallback& run_script_callback);

  ~BackendApplication();

  void Start();
  void Stop();

  void RunScript(const base::FilePath& path);
  void RunScript(const std::string& source, const std::string& resource_name);

  base::WeakPtr<BackendApplication> GetWeakPtr() {
    return weak_factory_.GetWeakPtr();
  }

 private:
  void CreateEnvironment(
      v8::Isolate* isolate);  // must called on backend thread, once.
  void DisposeEnvironment();  // must called on backend thread.

  void OnThreadEnd(BackendThread* thread);

  void run_script_(const std::string& source, const std::string& resource_name);

 private:
  friend class BackendThread;

  scoped_refptr<base::TaskRunner> backend_runner_;
  scoped_refptr<base::TaskRunner> shell_runner_;
  scoped_ptr<zarun::Environment> environment_;
  scoped_ptr<BackendThread> main_thread_;
  base::Callback<void(BackendApplication*)> termination_callback_;
  scoped_ptr<BackendScriptContextDelegate> backend_context_delegate_;

  base::WeakPtrFactory<BackendApplication> weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(BackendApplication);
};
}
}  // namespace zarun::backend

#endif  // ZARUN_BACKEND_BACKEND_APPLICATION_H_
