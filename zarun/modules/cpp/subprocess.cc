/*
 * subprocess.cc
 *
 */

#include "zarun/modules/cpp/subprocess.h"

#include "base/bind.h"
#include "base/command_line.h"
#include "base/process/launch.h"
#include "gin/converter.h"
#include "gin/arguments.h"

#include "zarun/modules/cpp/internal/process_handle_wrapper.h"

namespace zarun {

void SubProcessNative::ProcessOpen(
    const v8::FunctionCallbackInfo<v8::Value>& info) {
  gin::Arguments args(info);
  if ((info.Length()) != 1 || (!info[0]->IsArray())) {
    return args.ThrowTypeError("args must be array");
  }

  std::vector<std::string> strargs;
  if (!args.GetNext(&strargs)) {
    return args.ThrowError();
  }

  base::CommandLine cmdline(strargs);
  base::LaunchOptions options;
  base::ProcessHandle process;

  bool ret = base::LaunchProcess(cmdline, options, &process);

  if (!ret) {
    args.isolate()->ThrowException(v8::Exception::Error(
        gin::StringToV8(args.isolate(), std::string("IOError"))));
    return args.Return(NULL);
  }

  gin::Handle<zarun::internal::ProcessHandleWrapper> handle =
      zarun::internal::ProcessHandleWrapper::Create(args.isolate(), process);
  args.Return(handle.ToV8());
}

SubProcessNative::SubProcessNative(ScriptContext* context)
    : ObjectBackedNativeModule(context) {
  RouteFunction("popen", base::Bind(&SubProcessNative::ProcessOpen,
                                    base::Unretained(this)));
}

SubProcessNative::~SubProcessNative() {
  Invalidate();
}

}  // namespace zarun
