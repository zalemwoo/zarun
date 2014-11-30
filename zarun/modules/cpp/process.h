/*
 * process.h
 *
 */

#ifndef ZARUN_MODULES_PROCESS_H_
#define ZARUN_MODULES_PROCESS_H_

#include "gin/handle.h"
#include "gin/wrappable.h"
#include "v8/include/v8.h"

#include "zarun/zarun_export.h"

namespace zarun {

// The Console module provides a basic API for printing to stdout. Over time,
// we'd like to evolve the API to match window.console in browsers.
class ZARUN_EXPORT Process : public gin::Wrappable<Process> {
 public:
  static const char kModuleName[];

  static gin::Handle<Process> Create(v8::Isolate* isolate);
  gin::ObjectTemplateBuilder GetObjectTemplateBuilder(
      v8::Isolate* isolate) override;

  static gin::WrapperInfo kWrapperInfo;
};

}  // namespace zarun

#endif  // ZARUN_MODULES_PROCESS_H_
