// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "zarun/modules/native_javascript_module.h"

namespace zarun {

NativeJavaScriptModule::NativeJavaScriptModule() : is_valid_(true) {
}

NativeJavaScriptModule::~NativeJavaScriptModule() {
}

void NativeJavaScriptModule::Invalidate() {
  is_valid_ = false;
}

}  // namespace zarun
