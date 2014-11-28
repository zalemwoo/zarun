// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "zarun/modules/cpp/print.h"

#include <string>

#include "base/bind.h"
#include "base/strings/string_util.h"
#include "gin/per_context_data.h"

namespace zarun {

const char PrintModule::kModuleName[] = "print";

PrintModule::PrintModule(ScriptContext* context)
    : ObjectBackedNativeModule(context) {
  RouteFunction("Print",
                base::Bind(&PrintModule::Print, base::Unretained(this)));
}

void PrintModule::Print(const v8::FunctionCallbackInfo<v8::Value>& args) {
  if (args.Length() < 1)
    return;

  std::vector<std::string> components;
  for (int i = 0; i < args.Length(); ++i)
    components.push_back(*v8::String::Utf8Value(args[i]));

  LOG(ERROR) << JoinString(components, ',');
}

}  // namespace zarun
