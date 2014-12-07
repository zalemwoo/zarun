/*
 * Throw_exception.h
 *
 */

// take from webkit/core/dom/V8ThrowException.h

#ifndef ZARUN_EXCEPTION_V8_THROW_EXCEPTION
#define ZARUN_EXCEPTION_V8_THROW_EXCEPTION

#include <string>

#include "v8/include/v8.h"

namespace zarun {

class ScriptContext;

class V8ThrowException {
 public:
  static v8::Handle<v8::Value> CreateException(ScriptContext* context,
                                               int errorcode,
                                               const char* message,
                                               int errorno = 0,
                                               const char* api = nullptr,
                                               const char* path = nullptr);
  static v8::Handle<v8::Value> ThrowException(ScriptContext* context,
                                              int errorcode,
                                              const char* message,
                                              int errorno = 0,
                                              const char* api = nullptr,
                                              const char* path = nullptr);
  static v8::Handle<v8::Value> ThrowException(v8::Isolate*,
                                              v8::Handle<v8::Value>);
  static v8::Handle<v8::Value> CreateGeneralError(v8::Isolate* isolate,
                                                  const std::string& message);
  static v8::Handle<v8::Value> ThrowGeneralError(v8::Isolate* isolate,
                                                 const std::string& message);
  static v8::Handle<v8::Value> CreateTypeError(v8::Isolate* isolate,
                                               const std::string& message);
  static v8::Handle<v8::Value> ThrowTypeError(v8::Isolate* isolate,
                                              const std::string& message);
  static v8::Handle<v8::Value> CreateRangeError(v8::Isolate* isolate,
                                                const std::string& message);
  static v8::Handle<v8::Value> ThrowRangeError(v8::Isolate* isolate,
                                               const std::string& message);
  static v8::Handle<v8::Value> CreateSyntaxError(v8::Isolate* isolate,
                                                 const std::string& message);
  static v8::Handle<v8::Value> ThrowSyntaxError(v8::Isolate* isolate,
                                                const std::string& message);
  static v8::Handle<v8::Value> CreateReferenceError(v8::Isolate* isolate,
                                                    const std::string& message);
  static v8::Handle<v8::Value> ThrowReferenceError(v8::Isolate* isolate,
                                                   const std::string& message);
};

}  // namespace zarun::v8

#endif  // ZARUN_EXCEPTION_V8_THROW_EXCEPTION
