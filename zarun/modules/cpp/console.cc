// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "zarun/modules/cpp/console.h"

#include <iostream>

#include "base/strings/string_util.h"
#include "gin/arguments.h"
#include "gin/converter.h"
#include "gin/object_template_builder.h"
#include "gin/per_isolate_data.h"
#include "gin/public/wrapper_info.h"

#include "zarun/console.h"

namespace zarun {

const char Console::kModuleName[] = "console";

// static
v8::Local<v8::Value> Console::GetModule(v8::Isolate* isolate) {
  return zarun::console::AsV8Object(isolate);
}

}  // namespace zarun
