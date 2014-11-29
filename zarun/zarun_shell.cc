#include "zarun/zarun_shell.h"
#include "zarun/switches.h"
#include "zarun/line_editor.h"
#include "zarun/backend/backend_application.h"

#include "base/bind.h"
#include "base/lazy_instance.h"
#include "base/callback_helpers.h"
#include "base/files/file_path.h"
#include "base/location.h"
#include "base/logging.h"

#include <iostream>

namespace zarun {

namespace {

static const std::string kDefaultV8Options =
    "--harmony --es_staging --expose_gc";

base::LazyInstance<zarun::ZarunShell> zarun_shell = LAZY_INSTANCE_INITIALIZER;


}  // namespace

// static
ZarunShell& ZarunShell::GetZarunShell() {
  return zarun_shell.Get();
}

ZarunShell::ZarunShell() {
  const base::CommandLine* command_line =
      base::CommandLine::ForCurrentProcess();

  if (command_line->HasSwitch(zarun::switches::kRepl)) {
    shell_mode_ = ShellMode::Repl;
  }
}

ZarunShell::~ZarunShell() {
  if (shell_mode_ == ShellMode::Repl) {
    LineEditor* console = LineEditor::Get();
    console->Close();
  }
}

void ZarunShell::Init() {
  if (shell_mode_ == ShellMode::Repl) {
    LineEditor* console = LineEditor::Get();
    console->Open();
  }
}

void ZarunShell::Run() {
  base::MessageLoop message_loop;

  base::RunLoop run_loop;
  quit_closure_ = run_loop.QuitClosure();

  task_runner_ = base::MessageLoopProxy::current().get();

  // BackendScriptRunnerDelegate owned by backend_application, will freed by it.
  backend_application_.reset(new zarun::backend::BackendApplication(
      task_runner_,
      base::Bind(&ZarunShell::OnBackendApplicationEnd, base::Unretained(this)),
      base::Bind(&ZarunShell::OnDidRunScript, base::Unretained(this))));

  backend_application_->Start();

  const base::CommandLine* command_line =
      base::CommandLine::ForCurrentProcess();

  base::CommandLine::StringVector argv = command_line->GetArgs();
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
  DLOG(INFO) << "bye.";
}

void ZarunShell::Repl() {
  LineEditor* console = LineEditor::Get();
  while (true) {
    std::string script = console->Prompt("> ");
    if (script.empty())
      continue;
    if (script == "q") {
      backend_application_->Stop();
      break;
    } else {
      backend_application_->RunScript(script, "zarun_repl");
      break;
    }
  }
}

void ZarunShell::OnDidRunScript(std::string result) {
  std::cerr << result << std::endl << std::flush;
  task_runner_->PostTask(FROM_HERE,
                         base::Bind(&ZarunShell::Repl, base::Unretained(this)));
}

void ZarunShell::OnBackendApplicationEnd(
    zarun::backend::BackendApplication* backendApplication) {
  task_runner_->PostTask(FROM_HERE, quit_closure_);
}

const std::string& GetDefaultV8Options() {
  return kDefaultV8Options;
}

}  // namespace zarun
