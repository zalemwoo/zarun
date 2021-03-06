/*
 * console.cc
 *
 */

#include "zarun/console.h"

#include <ostream>
#include <iostream>

#include "base/compiler_specific.h"
#include "base/debug/alias.h"
#include "base/lazy_instance.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"

#include "zarun/utils/standard_out.h"

namespace zarun {
namespace console {

namespace {

void AddMessage(v8::Handle<v8::Context> context,
                console::ConsoleMessageLevel level,
                const std::string& message) {
  enum zarun::util::TextDecoration decoration;
  switch (level) {
    case console::ConsoleMessageLevel::CONSOLE_MESSAGE_LEVEL_DEBUG:
      decoration = zarun::util::TextDecoration::DECORATION_BLUE;
      break;
    case console::ConsoleMessageLevel::CONSOLE_MESSAGE_LEVEL_LOG:
      decoration = zarun::util::TextDecoration::DECORATION_GREEN;
      break;
    case console::ConsoleMessageLevel::CONSOLE_MESSAGE_LEVEL_WARNING:
      decoration = zarun::util::TextDecoration::DECORATION_YELLOW;
      break;
    case console::ConsoleMessageLevel::CONSOLE_MESSAGE_LEVEL_ERROR:
      decoration = zarun::util::TextDecoration::DECORATION_RED;
      break;
    default:
      decoration = zarun::util::TextDecoration::DECORATION_NONE;
      break;
  }
  zarun::util::OutputString(message + "\n", decoration);
}

// Writes |message| to stack to show up in minidump, then crashes.
void CheckWithMinidump(const std::string& message) {
  char minidump[1024];
  base::debug::Alias(&minidump);
  base::snprintf(minidump, arraysize(minidump), "e::console: %s",
                 message.c_str());
  CHECK(false) << message;
}

typedef void (*LogMethod)(v8::Handle<v8::Context> context,
                          const std::string& message);

void BoundLogMethodCallback(const v8::FunctionCallbackInfo<v8::Value>& info) {
  LogMethod log_method =
      reinterpret_cast<LogMethod>(info.Data().As<v8::External>()->Value());
  std::vector<std::string> strs;
  for (int i = 0; i < info.Length(); i++) {
    v8::Handle<v8::Value> val;
    strs.push_back(*v8::String::Utf8Value(info[i]));
  }

  (*log_method)(info.GetIsolate()->GetCallingContext(), JoinString(strs, ','));
}

void BindLogMethod(v8::Isolate* isolate,
                   v8::Local<v8::Object> target,
                   const std::string& name,
                   LogMethod log_method) {
  v8::Local<v8::FunctionTemplate> tmpl = v8::FunctionTemplate::New(
      isolate, &BoundLogMethodCallback,
      v8::External::New(isolate, reinterpret_cast<void*>(log_method)));
  target->Set(v8::String::NewFromUtf8(isolate, name.c_str()),
              tmpl->GetFunction());
}

}  // namespace

void Debug(v8::Handle<v8::Context> context, const std::string& message) {
  AddMessage(context, console::CONSOLE_MESSAGE_LEVEL_DEBUG, message);
}

void Log(v8::Handle<v8::Context> context, const std::string& message) {
  AddMessage(context, console::CONSOLE_MESSAGE_LEVEL_LOG, message);
}

void Warn(v8::Handle<v8::Context> context, const std::string& message) {
  AddMessage(context, console::CONSOLE_MESSAGE_LEVEL_WARNING, message);
}

void Error(v8::Handle<v8::Context> context, const std::string& message) {
  AddMessage(context, console::CONSOLE_MESSAGE_LEVEL_ERROR, message);
}

void Fatal(v8::Handle<v8::Context> context, const std::string& message) {
  Error(context, message);
  CheckWithMinidump(message);
}

v8::Local<v8::Object> AsV8Object(v8::Isolate* isolate) {
  v8::EscapableHandleScope handle_scope(isolate);
  v8::Local<v8::Object> console_object = v8::Object::New(isolate);
  BindLogMethod(isolate, console_object, "debug", &Debug);
  BindLogMethod(isolate, console_object, "log", &Log);
  BindLogMethod(isolate, console_object, "warn", &Warn);
  BindLogMethod(isolate, console_object, "error", &Error);
  return handle_scope.Escape(console_object);
}
}
}  // namespace zarun::console
