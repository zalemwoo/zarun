// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// this code is took from chromium/src/tools/gn

#ifndef ZARUN_UTILS_STANDARD_OUT_H_
#define ZARUN_UTILS_STANDARD_OUT_H_

#include <string>

namespace zarun {
namespace util {

enum TextDecoration {
  DECORATION_NONE = 0,
  DECORATION_RED,
  DECORATION_GREEN,
  DECORATION_BLUE,
  DECORATION_YELLOW
};

void OutputString(const std::string& output,
                  TextDecoration dec = DECORATION_NONE);
}
}  // namespcae zarun::util

#endif  // ZARUN_UTILS_STANDARD_OUT_H_
