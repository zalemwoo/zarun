#include "zarun/zarun_shell.h"
#include "zarun/switches.h"
#include "zarun/line_editor.h"
#include "zarun/backend/backend_runner_delegate.h"

#include "base/bind.h"
#include "base/callback_helpers.h"
#include "base/files/file_path.h"
#include "base/location.h"
#include "base/logging.h"

#include "v8/include/v8.h"

#include <iostream>

namespace zarun {

namespace {

void ProcessScriptResult(v8::Handle<v8::Value> result) {
  std::cerr << *v8::String::Utf8Value(result) << std::endl;
  std::cout << "> " << std::flush;
}

}  // namespace

ZarunShell::ZarunShell()
    : cmd_line_args_(NULL), shell_mode_(ShellMode::Batch) {}

ZarunShell::~ZarunShell() {}

void ZarunShell::Init(const base::CommandLine* args) {
  cmd_line_args_ = args;
  if (cmd_line_args_->HasSwitch(zarun::switches::kRepl)) {
    shell_mode_ = ShellMode::Repl;
  }
}

void ZarunShell::Run() {
  base::MessageLoop message_loop;

  base::RunLoop run_loop;
  quit_closure_ = run_loop.QuitClosure();

  task_runner_ = base::MessageLoopProxy::current().get();

  backend_application_.reset(new zarun::backend::BackendApplication(
      task_runner_,
      base::Bind(&ZarunShell::OnBackendApplicationEnd, base::Unretained(this)),
      new backend::BackendScriptRunnerDelegate(
          base::Bind(&ProcessScriptResult))));

  backend_application_->Start();

  base::CommandLine::StringVector argv = cmd_line_args_->GetArgs();
  for (CommandLine::StringVector::const_iterator it = argv.begin();
       it != argv.end(); ++it) {
    backend_application_->RunScript(base::FilePath(*it));
  }

  if (shell_mode_ == ShellMode::Repl) {
    task_runner_->PostTask(
        FROM_HERE, base::Bind(&ZarunShell::Repl, base::Unretained(this)));
  } else {
    backend_application_->Stop();
  }

  run_loop.Run();
}

void ZarunShell::Repl() {
  LineEditor* console = LineEditor::Get();
  console->Open();
  std::string script;
  while (true) {
    script = console->Prompt("= ");
    if (script.empty()) continue;
    if (script == "q") {
      backend_application_->Stop();
      break;
    } else {
      backend_application_->RunScript(script, "zarun_repl");
    }
  }
  console->Close();
  DLOG(INFO) << "bye.";
}

void ZarunShell::OnBackendApplicationEnd(
    zarun::backend::BackendApplication* backendApplication) {
  task_runner_->PostTask(FROM_HERE, quit_closure_);
}
}
// namespace zarun
