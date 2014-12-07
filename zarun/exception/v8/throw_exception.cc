/*
 * Throw_exception.cc
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

v8::Handle<v8::Value> V8ThrowException::CreateException(ScriptContext* context,
                                                        int errorcode,
                                                        const char* message,
                                                        int errorno,
                                                        const char* api,
                                                        const char* path) {
  if (errorcode <= 0 || v8::V8::IsExecutionTerminating())
    return v8::Handle<v8::Value>();

  v8::Isolate* isolate = context->isolate();

  std::string error_message;

  if (errorcode == ZarunErrnoError) {
    std::stringstream ss;
    if (message != NULL && message[0] != '\0') {
      ss << message << ": ";
    }
    ss << strerror(errorno) << ": " << errno_string(errorno) << "(" << errorno
       << ")";
    error_message = ss.str();
  } else {
    error_message = std::string(message ? message : "");
  }

  if (errorcode == V8GeneralError)
    return V8ThrowException::CreateGeneralError(isolate, error_message);
  if (errorcode == V8TypeError)
    return V8ThrowException::CreateTypeError(isolate, error_message);
  if (errorcode == V8RangeError)
    return V8ThrowException::CreateRangeError(isolate, error_message);
  if (errorcode == V8SyntaxError)
    return V8ThrowException::CreateSyntaxError(isolate, error_message);
  if (errorcode == V8ReferenceError)
    return V8ThrowException::CreateReferenceError(isolate, error_message);

  gin::Handle<ZarunExceptionWrapper> exception_handle =
      ZarunExceptionWrapper::Create(context, errorcode, error_message.c_str());
  v8::Handle<v8::Value> exception = exception_handle.ToV8();
  if (exception.IsEmpty()) {
    return v8::Handle<v8::Value>();
  }

  ZarunExceptionWrapper* ze = exception_handle.get();
  if (api) {
    ze->set_api(api);
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

v8::Handle<v8::Value> V8ThrowException::ThrowException(ScriptContext* context,
                                                       int errorcode,
                                                       const char* message,
                                                       int errorno,
                                                       const char* api,
                                                       const char* path) {
  v8::Handle<v8::Value> exception =
      CreateException(context, errorcode, message, errorno, api, path);
  if (exception.IsEmpty())
    return v8::Handle<v8::Value>();
  return V8ThrowException::ThrowException(context->isolate(), exception);
}

v8::Handle<v8::Value> V8ThrowException::CreateGeneralError(
    v8::Isolate* isolate,
    const std::string& message) {
  return v8::Exception::Error(
      gin::StringToV8(isolate, message.empty() ? "Error" : message));
}

v8::Handle<v8::Value> V8ThrowException::ThrowGeneralError(
    v8::Isolate* isolate,
    const std::string& message) {
  v8::Handle<v8::Value> exception =
      V8ThrowException::CreateGeneralError(isolate, message);
  return V8ThrowException::ThrowException(isolate, exception);
}

v8::Handle<v8::Value> V8ThrowException::CreateTypeError(
    v8::Isolate* isolate,
    const std::string& message) {
  return v8::Exception::TypeError(
      gin::StringToV8(isolate, message.empty() ? "Type error" : message));
}

v8::Handle<v8::Value> V8ThrowException::ThrowTypeError(
    v8::Isolate* isolate,
    const std::string& message) {
  v8::Handle<v8::Value> exception =
      V8ThrowException::CreateTypeError(isolate, message);
  return V8ThrowException::ThrowException(isolate, exception);
}

v8::Handle<v8::Value> V8ThrowException::CreateRangeError(
    v8::Isolate* isolate,
    const std::string& message) {
  return v8::Exception::RangeError(
      gin::StringToV8(isolate, message.empty() ? "Range error" : message));
}

v8::Handle<v8::Value> V8ThrowException::ThrowRangeError(
    v8::Isolate* isolate,
    const std::string& message) {
  v8::Handle<v8::Value> exception =
      V8ThrowException::CreateRangeError(isolate, message);
  return V8ThrowException::ThrowException(isolate, exception);
}

v8::Handle<v8::Value> V8ThrowException::CreateSyntaxError(
    v8::Isolate* isolate,
    const std::string& message) {
  return v8::Exception::SyntaxError(
      gin::StringToV8(isolate, message.empty() ? "Syntax error" : message));
}

v8::Handle<v8::Value> V8ThrowException::ThrowSyntaxError(
    v8::Isolate* isolate,
    const std::string& message) {
  v8::Handle<v8::Value> exception =
      V8ThrowException::CreateSyntaxError(isolate, message);
  return V8ThrowException::ThrowException(isolate, exception);
}

v8::Handle<v8::Value> V8ThrowException::CreateReferenceError(
    v8::Isolate* isolate,
    const std::string& message) {
  return v8::Exception::ReferenceError(
      gin::StringToV8(isolate, message.empty() ? "Reference error" : message));
}

v8::Handle<v8::Value> V8ThrowException::ThrowReferenceError(
    v8::Isolate* isolate,
    const std::string& message) {
  v8::Handle<v8::Value> exception =
      V8ThrowException::CreateReferenceError(isolate, message);
  return V8ThrowException::ThrowException(isolate, exception);
}

v8::Handle<v8::Value> V8ThrowException::ThrowException(
    v8::Isolate* isolate,
    v8::Handle<v8::Value> exception) {
  if (!v8::V8::IsExecutionTerminating())
    isolate->ThrowException(exception);
  return v8::Undefined(isolate);
}

}  // namespace zarun
