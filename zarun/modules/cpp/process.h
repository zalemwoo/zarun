/*
 * process.h
 *
 */

#ifndef ZARUN_MODULES_PROCESS_H_
#define ZARUN_MODULES_PROCESS_H_

#include "v8/include/v8.h"

#include "zarun/zarun_export.h"

namespace zarun {

// The Console module provides a basic API for printing to stdout. Over time,
// we'd like to evolve the API to match window.console in browsers.
class ZARUN_EXPORT Process {
 public:
  static const char kModuleName[];
  static v8::Local<v8::Value> GetModule(v8::Isolate* isolate);
};

}  // namespace zarun

#endif /* ZARUN_MODULES_PROCESS_H_ */
