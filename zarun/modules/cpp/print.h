// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ZARUN_MODULES_PRINT_H_
#define ZARUN_MODULES_PRINT_H_

#include "zarun/modules/object_backed_native_module.h"

namespace zarun {

class PrintModule : public ObjectBackedNativeModule {
 public:
  static const char kModuleName[];
  static v8::Local<v8::Value> GetModule(v8::Isolate* isolate);

  explicit PrintModule(ScriptContext* context);
  void Print(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}  // namespace zarun

#endif  // ZARUN_MODULES_PRINT_H_
