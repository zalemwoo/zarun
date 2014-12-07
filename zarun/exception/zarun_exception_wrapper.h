/*
 * zarun_exception_wrapper.h
 *
 */

#ifndef ZARUN_EXCEPTION_WRAPPER_H_
#define ZARUN_EXCEPTION_WRAPPER_H_

#include <string>

#include "zarun/modules/native_object.h"

namespace zarun {

class ScriptContext;

typedef int ExceptionCode;

DECLARE_NATIVE_OBJECT(ZarunExceptionWrapper)
DECLARE_NATIVE_OBJECT_START(ZarunExceptionWrapper)

public:
static gin::Handle<ZarunExceptionWrapper> Create(ScriptContext* context,
                                                 int code,
                                                 const char* message);

private:
ZarunExceptionWrapper(ScriptContext* context,
                      unsigned short code,
                      const char* name,
                      const char* message);

public:

unsigned short code() const {
  return code_;
}
std::string name() const {
  return name_;
}
std::string message() const {
  return message_;
}
std::string api() const {
  return api_;
}
void set_api(const char* api) {
  api_ = api;
}
std::string path() const {
  return path_;
}
void set_path(const char* path) {
  path_ = path;
}

static const char* getErrorName(ExceptionCode);
static const char* getErrorMessage(ExceptionCode);

private:
unsigned short code_;
std::string name_;
std::string message_;
std::string api_;
std::string path_;

DECLARE_NATIVE_OBJECT_END

}  // namespace zarun

#endif  // ZARUN_EXCEPTION_WRAPPER_H_
