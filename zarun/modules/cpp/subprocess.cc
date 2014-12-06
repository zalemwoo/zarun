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
#include "gin/object_template_builder.h"

#include "zarun/script_context.h"

#include "zarun/modules/cpp/process.h"

namespace zarun {

// static
void SubProcessNative::ProcessOpenCallback(gin::Arguments* args) {
  std::vector<std::string> strargs;
  if (!args->GetNext(&strargs)) {
    return args->ThrowError();
  }

  base::CommandLine cmdline(strargs);
  base::LaunchOptions options;
  base::ProcessHandle process_handle;

  bool ret = base::LaunchProcess(cmdline, options, &process_handle);

  if (!ret) {
    args->isolate()->ThrowException(v8::Exception::Error(
        gin::StringToV8(args->isolate(), std::string("IOError"))));
    return;
  }

  ScriptContext* context =
      ScriptContext::FromV8Context(args->isolate()->GetCurrentContext());

  gin::Handle<zarun::ProcessNative> process =
      zarun::ProcessNative::Create(context, process_handle);
  args->Return(process.ToV8());
}

gin::WrapperInfo SubProcessNative::kWrapperInfo = {gin::kEmbedderNativeGin};
const char SubProcessNative::kModuleName[] = "subprocess";

SubProcessNative::SubProcessNative(ScriptContext* context)
    : ThinNativeModule<SubProcessNative>(context) {
}

SubProcessNative::~SubProcessNative() {
}

gin::ObjectTemplateBuilder SubProcessNative::GetObjectTemplateBuilder(
    v8::Isolate* isolate) {
  return ThinNativeModule<SubProcessNative>::GetObjectTemplateBuilder(isolate)
      .SetMethod("popen", &SubProcessNative::ProcessOpenCallback);
}

void SubProcessNative::Invalidate() {
  ThinNativeModule<SubProcessNative>::Invalidate();
}

}  // namespace zarun
