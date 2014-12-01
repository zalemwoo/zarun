// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "zarun/utils/standard_out.h"

#include <vector>

#include "base/command_line.h"
#include "base/logging.h"
#include "base/strings/string_split.h"
#include "build/build_config.h"
#include "zarun/switches.h"

#if defined(OS_WIN)
#include <windows.h>
#else
#include <stdio.h>
#include <unistd.h>
#endif

namespace {

bool initialized = false;

#if defined(OS_WIN)
HANDLE hstdout;
WORD default_attributes;
#endif
bool is_console = false;

void EnsureInitialized() {
  if (initialized)
    return;
  initialized = true;

  const base::CommandLine* cmdline = base::CommandLine::ForCurrentProcess();
  if (cmdline->HasSwitch(zarun::switches::kNoColor)) {
    // Force color off.
    is_console = false;
    return;
  }

#if defined(OS_WIN)
  // On Windows, we can't force the color on. If the output handle isn't a
  // console, there's nothing we can do about it.
  hstdout = ::GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO info;
  is_console = !!::GetConsoleScreenBufferInfo(hstdout, &info);
  default_attributes = info.wAttributes;
#else
  if (cmdline->HasSwitch(zarun::switches::kColor))
    is_console = true;
  else
    is_console = isatty(fileno(stdout));
#endif
}

void WriteToStdOut(const std::string& output) {
  size_t written_bytes = fwrite(output.data(), 1, output.size(), stdout);
  DCHECK_EQ(output.size(), written_bytes);
}

}  // namespace

namespace zarun {
namespace util {

#if defined(OS_WIN)

void OutputString(const std::string& output, TextDecoration dec) {
  EnsureInitialized();
  if (is_console) {
    switch (dec) {
      case DECORATION_NONE:
        break;
      case DECORATION_DIM:
        ::SetConsoleTextAttribute(hstdout, FOREGROUND_INTENSITY);
        break;
      case DECORATION_RED:
        ::SetConsoleTextAttribute(hstdout,
                                  FOREGROUND_RED | FOREGROUND_INTENSITY);
        break;
      case DECORATION_GREEN:
        // Keep green non-bold.
        ::SetConsoleTextAttribute(hstdout, FOREGROUND_GREEN);
        break;
      case DECORATION_BLUE:
        ::SetConsoleTextAttribute(hstdout,
                                  FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        break;
      case DECORATION_YELLOW:
        ::SetConsoleTextAttribute(hstdout, FOREGROUND_RED | FOREGROUND_GREEN);
        break;
    }
  }

  DWORD written = 0;
  ::WriteFile(hstdout, output.c_str(), static_cast<DWORD>(output.size()),
              &written, NULL);

  if (is_console)
    ::SetConsoleTextAttribute(hstdout, default_attributes);
}

#else

void OutputString(const std::string& output, TextDecoration dec) {
  EnsureInitialized();
  if (is_console) {
    switch (dec) {
      case DECORATION_NONE:
        break;
      case DECORATION_DIM:
        WriteToStdOut("\e[2m");
        break;
      case DECORATION_RED:
        WriteToStdOut("\e[31m\e[1m");
        break;
      case DECORATION_GREEN:
        WriteToStdOut("\e[32m");
        break;
      case DECORATION_BLUE:
        WriteToStdOut("\e[34m\e[1m");
        break;
      case DECORATION_YELLOW:
        WriteToStdOut("\e[33m\e[1m");
        break;
    }
  }

  WriteToStdOut(output.data());

  if (is_console && dec != DECORATION_NONE)
    WriteToStdOut("\e[0m");
}

#endif
}
}  // namespcae zarun::util

