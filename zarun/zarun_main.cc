#include <iostream>

#include "base/logging.h"
#include "base/at_exit.h"
#include "base/bind.h"
#include "base/command_line.h"
#include "base/files/file_util.h"
#include "base/i18n/icu_util.h"
#include "base/run_loop.h"
#include "base/message_loop/message_loop.h"

#include "gin/public/isolate_holder.h"

#include "zarun/zarun_shell.h"
#include "zarun/switches.h"

void Usage() {
  std::cerr << "Zarun, for study base/gin/mojo/v8... from chromium codebase.\n";
  std::cerr << "Usage: zarun"
            << " [--" << zarun::switches::kRepl << "]"
            << " ...\n";
}

int main(int argc, char** argv) {
  base::AtExitManager at_exit;
  CommandLine::Init(argc, argv);
  base::i18n::InitializeICU();
#ifdef V8_USE_EXTERNAL_STARTUP_DATA
  gin::IsolateHolder::LoadV8Snapshot();
#endif

  const base::CommandLine* command_line =
      base::CommandLine::ForCurrentProcess();

  if (!command_line->HasSwitch(zarun::switches::kRepl) &&
      command_line->GetArgs().empty()) {
    Usage();
    return 0;
  }

  zarun::ZarunShell& zarun_shell = zarun::ZarunShell::GetZarunShell();
  zarun_shell.Init(command_line);
  zarun_shell.Run();

  return 0;
}
