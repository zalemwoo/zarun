#ifndef ZARUN_SHELL_H_
#define ZARUN_SHELL_H_

#include <string>

#include "base/command_line.h"
#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/single_thread_task_runner.h"
#include "base/synchronization/waitable_event.h"
#include "base/threading/thread.h"
#include "base/threading/thread_checker.h"
#include "base/run_loop.h"

namespace zarun {

namespace backend {
class BackendApplication;
}

enum ShellMode {
  Batch = 0,
  Repl,
};

class ZarunShell {
 public:
  static ZarunShell& GetZarunShell();
  static ShellMode Mode() { return GetZarunShell().shell_mode_; }

 public:
  ZarunShell();
  ~ZarunShell();
  void Init(const base::CommandLine* args);
  void Run();

 private:
  void Repl();
  void OnBackendApplicationEnd(
      zarun::backend::BackendApplication* backendApplication);

 private:
  scoped_ptr<zarun::backend::BackendApplication> backend_application_;
  scoped_refptr<base::TaskRunner> task_runner_;
  const base::CommandLine* cmd_line_args_;
  ShellMode shell_mode_;

  base::Closure quit_closure_;

  DISALLOW_COPY_AND_ASSIGN(ZarunShell);
};

const std::string& GetDefaultV8Options();

}  // namespace zarun

#endif  // ZARUN_SHELL_H_
