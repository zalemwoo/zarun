/*
 * module_provider.cc
 *
 */

#include "zarun/modules/module_provider.h"

#include "base/bind.h"
#include "base/logging.h"

namespace zarun {

ModuleProvider::ModuleProvider() {
}

ModuleProvider::~ModuleProvider() {
}

void ModuleProvider::RegisterModule(const std::string& id,
                                    ModuleGetter getter) {
  registered_modules_[id] = base::Bind(getter);
}

void ModuleProvider::RegisterModule(const std::string& id,
                                    const ModuleGetterCallback& getter) {
  registered_modules_[id] = getter;
}

}  // namespace zarun
