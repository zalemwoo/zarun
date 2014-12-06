/*
 * process.h
 *
 */

#ifndef ZARUN_MODULES_CPP_PROCESS_H_
#define ZARUN_MODULES_CPP_PROCESS_H_

#include "base/memory/scoped_ptr.h"
#include "base/process/process_handle.h"
#include "base/process/process.h"

#include "zarun/modules/native_object.h"

namespace zarun {

DECLARE_NATIVE_OBJECT(ProcessNative)

private:
ProcessNative(ScriptContext* context, base::ProcessHandle process_handle);

public:
  void CloseCallback();
  void IsValidCallback(gin::Arguments* args);

 private:
  scoped_ptr<base::Process> process_;

  DECLARE_NATIVE_OBJECT_END

}  // namespace zarun

#endif  // ZARUN_MODULES_CPP_PROCESS_H_
