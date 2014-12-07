/*
 * throw_exception.h
 *
 */

// take from webkit/core/dom/V8ThrowException.h

#ifndef ZARUN_EXCEPTION_V8_THROW_EXCEPTION
#define ZARUN_EXCEPTION_V8_THROW_EXCEPTION

#include <string>

#include "v8/include/v8.h"

namespace zarun {

class ScriptContext;

class ThrowException {
 public:
  static v8::Handle<v8::Value> createException(ScriptContext* context,
                                               int errorcode,
                                               const char* message,
                                               int errorno = 0,
                                               const char* syscall = nullptr,
                                               const char* path = nullptr);
  static v8::Handle<v8::Value> throwException(ScriptContext* context,
                                              int errorcode,
                                              const char* message,
                                              int errorno = 0,
                                              const char* syscall = nullptr,
                                              const char* path = nullptr);
  static v8::Handle<v8::Value> throwException(v8::Isolate*,
                                              v8::Handle<v8::Value>);
  static v8::Handle<v8::Value> createGeneralError(v8::Isolate* isolate,
                                                  const std::string& message);
  static v8::Handle<v8::Value> throwGeneralError(v8::Isolate* isolate,
                                                 const std::string& message);
  static v8::Handle<v8::Value> createTypeError(v8::Isolate* isolate,
                                               const std::string& message);
  static v8::Handle<v8::Value> throwTypeError(v8::Isolate* isolate,
                                              const std::string& message);
  static v8::Handle<v8::Value> createRangeError(v8::Isolate* isolate,
                                                const std::string& message);
  static v8::Handle<v8::Value> throwRangeError(v8::Isolate* isolate,
                                               const std::string& message);
  static v8::Handle<v8::Value> createSyntaxError(v8::Isolate* isolate,
                                                 const std::string& message);
  static v8::Handle<v8::Value> throwSyntaxError(v8::Isolate* isolate,
                                                const std::string& message);
  static v8::Handle<v8::Value> createReferenceError(v8::Isolate* isolate,
                                                    const std::string& message);
  static v8::Handle<v8::Value> throwReferenceError(v8::Isolate* isolate,
                                                   const std::string& message);
};

}  // namespace zarun::v8

#endif  // ZARUN_EXCEPTION_V8_THROW_EXCEPTION
