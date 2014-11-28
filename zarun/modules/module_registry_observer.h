// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ZARUN_MODULES_MODULE_REGISTRY_OBSERVER_H_
#define ZARUN_MODULES_MODULE_REGISTRY_OBSERVER_H_

#include <string>
#include <vector>

#include "zarun/zarun_export.h"

namespace zarun {

// Notified of interesting events from ModuleRegistry.
class ZARUN_EXPORT ModuleRegistryObserver {
 public:
  // Called from AddPendingModule(). |id| is the id/name of the module and
  // |dependencies| this list of modules |id| depends upon.
  virtual void OnDidAddPendingModule(
      const std::string& id,
      const std::vector<std::string>& dependencies) = 0;

 protected:
  virtual ~ModuleRegistryObserver() {}
};

}  // namespace zarun

#endif  // ZARUN_MODULES_MODULE_REGISTRY_OBSERVER_H_
