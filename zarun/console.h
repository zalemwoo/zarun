/*
 * console.h
 *
 */

#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <string>

#include "v8/include/v8.h"

namespace zarun {
namespace console {

enum ConsoleMessageLevel {
  CONSOLE_MESSAGE_LEVEL_DEBUG,
  CONSOLE_MESSAGE_LEVEL_LOG,
  CONSOLE_MESSAGE_LEVEL_WARNING,
  CONSOLE_MESSAGE_LEVEL_ERROR,
  CONSOLE_MESSAGE_LEVEL_LAST = CONSOLE_MESSAGE_LEVEL_ERROR
};

void Debug(v8::Handle<v8::Context> context, const std::string& message);
void Log(v8::Handle<v8::Context> context, const std::string& message);
void Warn(v8::Handle<v8::Context> context, const std::string& message);
void Error(v8::Handle<v8::Context> context, const std::string& message);

// Logs an Error then crashes the current process.
void Fatal(v8::Handle<v8::Context> context, const std::string& message);

// Returns a new v8::Object with each standard log method (Debug/Log/Warn/Error)
// bound to respective debug/log/warn/error methods. This is a direct drop-in
// replacement for the standard devtools console.* methods usually accessible
// from JS.
v8::Local<v8::Object> AsV8Object(v8::Isolate* isolate);
}
}  // namespace zarun::console

#endif  // CONSOLE_H_
