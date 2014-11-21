// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ZAEUN_MODULES_CONSOLE_H_
#define ZAEUN_MODULES_CONSOLE_H_

#include "zarun/zarun_export.h"
#include "v8/include/v8.h"

namespace zarun {

// The Console module provides a basic API for printing to stdout. Over time,
// we'd like to evolve the API to match window.console in browsers.
class ZARUN_EXPORT Console {
 public:
  static const char kModuleName[];
  static v8::Local<v8::Value> GetModule(v8::Isolate* isolate);
};

}  // namespace zarun

#endif  // ZAEUN_MODULES_CONSOLE_H_
