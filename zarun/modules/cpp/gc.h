// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ZARUN_MODULES_GC_H_
#define ZARUN_MODULES_GC_H_

#include "v8/include/v8.h"

namespace zarun {

// This module provides bindings to the garbage collector.
class GC {
 public:
  static const char kModuleName[];
  static v8::Local<v8::Value> GetModule(v8::Isolate* isolate);
};

}  // namespace zarun

#endif  // ZARUN_MODULES_GC_H_
