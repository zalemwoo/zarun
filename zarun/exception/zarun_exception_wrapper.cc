/*
 * zarun_exception.cc
 *
 */

#include "zarun/exception/zarun_exception_wrapper.h"

#include "base/basictypes.h"
#include "base/logging.h"

#include "zarun/exception/exception_code.h"

namespace zarun {

namespace {

static const struct CoreException {
  const char* const name;
  const char* const message;
  const int code;
} coreExceptions[] = {
    {"IndexSizeError",
     "Index or size was negative, or greater than the allowed value.",
     1},
    {"GenericError", "Generic Error occurred.", 3},
};

static const CoreException* getErrorEntry(ExceptionCode ec) {
  size_t tableSize = arraysize(coreExceptions);
  size_t tableIndex = ec - IndexSizeError;
  return tableIndex < tableSize ? &coreExceptions[tableIndex]
                                : &coreExceptions[0];
}

}  // namespace

// static
gin::Handle<ZarunExceptionWrapper> ZarunExceptionWrapper::Create(
    ScriptContext* context,
    int code,
    const char* message) {
  const CoreException* entry = getErrorEntry(code);
  return WrappableNativeObject<ZarunExceptionWrapper>::Create(
      context, entry->code, entry->name ? entry->name : "Error",
      message ? message : entry->message);
}

DEFINE_WRAPPER_INFO(ZarunExceptionWrapper);

ZarunExceptionWrapper::ZarunExceptionWrapper(ScriptContext* context,
                                             unsigned short code,
                                             const char* name,
                                             const char* message)
    : WrappableNativeObject<ZarunExceptionWrapper>(context) {
  CHECK(name);
  code_ = code;
  name_ = name;
  message_ = message;
}

ZarunExceptionWrapper::~ZarunExceptionWrapper() {
}

gin::ObjectTemplateBuilder ZarunExceptionWrapper::GetObjectTemplateBuilder(
    v8::Isolate* isolate) {
  return WrappableNativeObject<ZarunExceptionWrapper>::GetObjectTemplateBuilder(
      isolate);
}

// static
const char* ZarunExceptionWrapper::getErrorName(ExceptionCode ec) {
  const CoreException* entry = getErrorEntry(ec);
  CHECK(entry);
  if (!entry)
    return "UnknownError";

  return entry->name;
}

// static
const char* ZarunExceptionWrapper::getErrorMessage(ExceptionCode ec) {
  const CoreException* entry = getErrorEntry(ec);
  CHECK(entry);
  if (!entry)
    return "Unknown error.";

  return entry->message;
}

}  // namespace zarun
