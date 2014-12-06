#include "zarun/modules/native_module.h"

namespace zarun {

void NativeModule::Invalidate() {
  this->is_valid_ = false;
}

}  // namespace zarun
