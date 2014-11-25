/*
 * backend_application.h
 *
 *  Created on: Nov 18, 2014
 *      Author: zalem
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

namespace zarun {
namespace backend {

class BackendScriptRunnerDelegate;

class ZARUN_EXPORT BackendApplication {
 public:
  BackendApplication(
      scoped_refptr<base::TaskRunner> shell_runner,
      const base::Callback<void(BackendApplication*)>& termination_callback,
      backend::BackendScriptRunnerDelegate* backend_script_runner_delegate);

  ~BackendApplication();

  void Start();
  void Stop();

  void RunScript(const base::FilePath& path);
  void RunScript(const std::string& source, const std::string& resource_name);

  base::WeakPtr<BackendApplication> GetWeakPtr() {
    return weak_factory_.GetWeakPtr();
  }

 private:
  void CreateEnvironment();
  void DisposeEnvironment();
  void OnThreadEnd(BackendThread* thread);

  void run_script_(const std::string& source, const std::string& resource_name);

 private:
  scoped_ptr<BackendScriptRunnerDelegate> backend_runner_delegate_;
  scoped_refptr<base::TaskRunner> backend_runner_;
  scoped_refptr<base::TaskRunner> shell_runner_;
  scoped_ptr<zarun::Environment> environment_;
  scoped_ptr<BackendThread> main_thread_;
  base::Callback<void(BackendApplication*)> termination_callback_;

  base::WeakPtrFactory<BackendApplication> weak_factory_;

  friend class BackendThread;
};
}
}  // namespace zarun::backend

#endif /* ZARUN_BACKEND_BACKEND_APPLICATION_H_ */
