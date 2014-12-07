/*
 * throw_exception.cc
 *
 */

#include "zarun/exception/v8/throw_exception.h"

#include <string.h>
#include <sstream>

#include "base/logging.h"
#include "gin/converter.h"

#include "zarun/script_context.h"
#include "zarun/exception/zarun_exception_wrapper.h"
#include "zarun/exception/exception_code.h"
#include "zarun/exception/errno_tostring.h"

namespace zarun {

namespace {
static void exceptionStackGetter(
    v8::Local<v8::String> name,
    const v8::PropertyCallbackInfo<v8::Value>& info) {
  CHECK(info.Data()->IsObject());
  info.GetReturnValue().Set(info.Data()->ToObject()->Get(
      gin::StringToV8(info.GetIsolate(), "stack")));
}

static void exceptionStackSetter(v8::Local<v8::String> name,
                                 v8::Local<v8::Value> value,
                                 const v8::PropertyCallbackInfo<void>& info) {
  CHECK(info.Data()->IsObject());
  info.Data()->ToObject()->Set(gin::StringToV8(info.GetIsolate(), "stack"),
                               value);
}

}  // namespace

v8::Handle<v8::Value> ThrowException::createException(ScriptContext* context,
                                                      int errorcode,
                                                      const char* message,
                                                      int errorno,
                                                      const char* syscall,
                                                      const char* path) {
  if (errorcode <= 0 || v8::V8::IsExecutionTerminating())
    return v8::Handle<v8::Value>();

  v8::Isolate* isolate = context->isolate();

  std::string error_message;

  if (errorcode == ZarunErrnoError) {
    std::stringstream ss;
    if (message == NULL || message[0] == '\0') {
      message = strerror(errorno);
    }
    ss << message << " : " << errno_string(errorno) << "(" << errorno << ")";
    error_message = ss.str();
  } else {
    error_message = std::string(message);
  }

  if (errorcode == V8GeneralError)
    return ThrowException::createGeneralError(isolate, error_message);
  if (errorcode == V8TypeError)
    return ThrowException::createTypeError(isolate, error_message);
  if (errorcode == V8RangeError)
    return ThrowException::createRangeError(isolate, error_message);
  if (errorcode == V8SyntaxError)
    return ThrowException::createSyntaxError(isolate, error_message);
  if (errorcode == V8ReferenceError)
    return ThrowException::createReferenceError(isolate, error_message);

  gin::Handle<ZarunExceptionWrapper> exception_handle =
      ZarunExceptionWrapper::Create(context, errorcode, error_message.c_str());
  v8::Handle<v8::Value> exception = exception_handle.ToV8();
  if (exception.IsEmpty()) {
    return v8::Handle<v8::Value>();
  }

  ZarunExceptionWrapper* ze = exception_handle.get();
  if (syscall) {
    ze->set_syscall(syscall);
  }
  if (path) {
    ze->set_path(path);
  }

  // Attach an Error object to the DOMException. This is then lazily used to get
  // the stack value.
  v8::Handle<v8::Value> error =
      v8::Exception::Error(gin::StringToV8(isolate, ze->message()));

  CHECK(!error.IsEmpty());
  CHECK(exception->IsObject());
  exception->ToObject()->SetAccessor(gin::StringToV8(isolate, "stack"),
                                     exceptionStackGetter, exceptionStackSetter,
                                     error);
  return exception;
}

v8::Handle<v8::Value> ThrowException::throwException(ScriptContext* context,
                                                     int errorcode,
                                                     const char* message,
                                                     int errorno,
                                                     const char* syscall,
                                                     const char* path) {
  v8::Handle<v8::Value> exception =
      createException(context, errorcode, message, errorno, syscall, path);
  if (exception.IsEmpty())
    return v8::Handle<v8::Value>();
  return ThrowException::throwException(context->isolate(), exception);
}

v8::Handle<v8::Value> ThrowException::createGeneralError(
    v8::Isolate* isolate,
    const std::string& message) {
  return v8::Exception::Error(
      gin::StringToV8(isolate, message.empty() ? "Error" : message));
}

v8::Handle<v8::Value> ThrowException::throwGeneralError(
    v8::Isolate* isolate,
    const std::string& message) {
  v8::Handle<v8::Value> exception =
      ThrowException::createGeneralError(isolate, message);
  return ThrowException::throwException(isolate, exception);
}

v8::Handle<v8::Value> ThrowException::createTypeError(
    v8::Isolate* isolate,
    const std::string& message) {
  return v8::Exception::TypeError(
      gin::StringToV8(isolate, message.empty() ? "Type error" : message));
}

v8::Handle<v8::Value> ThrowException::throwTypeError(
    v8::Isolate* isolate,
    const std::string& message) {
  v8::Handle<v8::Value> exception =
      ThrowException::createTypeError(isolate, message);
  return ThrowException::throwException(isolate, exception);
}

v8::Handle<v8::Value> ThrowException::createRangeError(
    v8::Isolate* isolate,
    const std::string& message) {
  return v8::Exception::RangeError(
      gin::StringToV8(isolate, message.empty() ? "Range error" : message));
}

v8::Handle<v8::Value> ThrowException::throwRangeError(
    v8::Isolate* isolate,
    const std::string& message) {
  v8::Handle<v8::Value> exception =
      ThrowException::createRangeError(isolate, message);
  return ThrowException::throwException(isolate, exception);
}

v8::Handle<v8::Value> ThrowException::createSyntaxError(
    v8::Isolate* isolate,
    const std::string& message) {
  return v8::Exception::SyntaxError(
      gin::StringToV8(isolate, message.empty() ? "Syntax error" : message));
}

v8::Handle<v8::Value> ThrowException::throwSyntaxError(
    v8::Isolate* isolate,
    const std::string& message) {
  v8::Handle<v8::Value> exception =
      ThrowException::createSyntaxError(isolate, message);
  return ThrowException::throwException(isolate, exception);
}

v8::Handle<v8::Value> ThrowException::createReferenceError(
    v8::Isolate* isolate,
    const std::string& message) {
  return v8::Exception::ReferenceError(
      gin::StringToV8(isolate, message.empty() ? "Reference error" : message));
}

v8::Handle<v8::Value> ThrowException::throwReferenceError(
    v8::Isolate* isolate,
    const std::string& message) {
  v8::Handle<v8::Value> exception =
      ThrowException::createReferenceError(isolate, message);
  return ThrowException::throwException(isolate, exception);
}

v8::Handle<v8::Value> ThrowException::throwException(
    v8::Isolate* isolate,
    v8::Handle<v8::Value> exception) {
  if (!v8::V8::IsExecutionTerminating())
    isolate->ThrowException(exception);
  return v8::Undefined(isolate);
}

}  // namespace zarun
