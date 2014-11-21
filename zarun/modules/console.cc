// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "zarun/modules/console.h"

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

gin::WrapperInfo g_wrapper_info = {gin::kEmbedderNativeGin};

}  // namespace

const char Console::kModuleName[] = "console";

v8::Local<v8::Value> Console::GetModule(v8::Isolate* isolate) {
  gin::PerIsolateData* data = gin::PerIsolateData::From(isolate);
  v8::Local<v8::ObjectTemplate> templ =
      data->GetObjectTemplate(&g_wrapper_info);
  if (templ.IsEmpty()) {
    templ = gin::ObjectTemplateBuilder(isolate).SetMethod("log", Log).Build();
    data->SetObjectTemplate(&g_wrapper_info, templ);
  }
  return templ->NewInstance();
}

}  // namespace zarun
