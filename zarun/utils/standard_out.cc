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

#include "third_party/termcolor/include/termcolor/termcolor.hpp"

#if defined(OS_WIN)
#include <windows.h>
#else
#include <stdio.h>
#include <unistd.h>
#endif

namespace {

bool initialized = false;
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
  HANDLE hstdout = ::GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO info;
  is_console = !!::GetConsoleScreenBufferInfo(hstdout, &info);
#else
  if (cmdline->HasSwitch(zarun::switches::kColor))
    is_console = true;
  else
    is_console = isatty(fileno(stdout));
#endif
}

}  // namespace

namespace zarun {
namespace util {

void OutputString(const std::string& output, TextDecoration dec) {
  EnsureInitialized();
  if (is_console) {
    switch (dec) {
      case DECORATION_NONE:
        break;
      case DECORATION_RED:
        std::cout << termcolor::red;
        break;
      case DECORATION_GREEN:
        std::cout << termcolor::green;
        break;
      case DECORATION_BLUE:
        std::cout << termcolor::blue;
        break;
      case DECORATION_YELLOW:
        std::cout << termcolor::yellow;
        break;
    }
  }

  std::cout << output.data() ;

  if (is_console && dec != DECORATION_NONE) {
    std::cout << termcolor::reset;
  }
}

}
}  // namespcae zarun::util

