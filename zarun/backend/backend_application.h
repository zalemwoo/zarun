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

#include "zarun/zarun_export.h"
#include "zarun/backend/backend_thread.h"

namespace zarun {
namespace backend {

class BackendScriptRunnerDelegate;

class ZARUN_EXPORT BackendApplication {
 public:
  BackendApplication(
      scoped_refptr<base::TaskRunner> shell_thread,
      const base::Callback<void(BackendApplication*)>& termination_callback,
      backend::BackendScriptRunnerDelegate* backend_script_runner_delegate);
  ~BackendApplication();

  void Start();
  void Stop();

  void RunScript(const base::FilePath& path);
  void RunScript(const std::string& source, const std::string& resource_name);

  void OnThreadEnd(BackendThread* thread);

 private:
  void run_script_(const std::string& source, const std::string& resource_name);

 private:
  BackendThread main_thread_;
  scoped_refptr<base::TaskRunner> task_runner_;
  scoped_refptr<base::TaskRunner> shell_thread_;
  base::Callback<void(BackendApplication*)> termination_callback_;

  DISALLOW_COPY_AND_ASSIGN(BackendApplication);
};
}
}  // namespace zarun::backend

#endif /* ZARUN_BACKEND_BACKEND_APPLICATION_H_ */
