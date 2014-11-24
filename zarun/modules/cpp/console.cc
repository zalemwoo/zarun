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

namespace zarun {

namespace {

static const char* ToCString(const v8::String::Utf8Value& value) {
  return *value ? *value : "<string conversion failed>";
}

void Log(gin::Arguments* args) {
  std::vector<std::string> strs;
  for (int i = 0; i < args->Length(); i++) {
    v8::Handle<v8::Value> val;
    if (!args->GetNext(&val)) {
      args->ThrowError();
      return;
    }
    strs.push_back(ToCString(v8::String::Utf8Value(val)));
  }
  std::cout << JoinString(strs, ' ') << std::endl;
}

}  // namespace

const char Console::kModuleName[] = "console";
gin::WrapperInfo Console::kWrapperInfo = {gin::kEmbedderNativeGin};

// static
gin::Handle<Console> Console::Create(v8::Isolate* isolate) {
  return gin::CreateHandle(isolate, new Console());
}

Console::Console() {}

Console::~Console() {}

// static
v8::Local<v8::Value> Console::GetModule(v8::Isolate* isolate) {
  return Create(isolate)->GetWrapper(isolate);
}

gin::ObjectTemplateBuilder Console::GetObjectTemplateBuilder(
    v8::Isolate* isolate) {
  return Wrappable<Console>::GetObjectTemplateBuilder(isolate)
      .SetMethod("log", Log);
}

}  // namespace zarun
