/*
 * zarun_exception_wrapper.h
 *
 */

#ifndef ZARUN_EXCEPTION_WRAPPER_H_
#define ZARUN_EXCEPTION_WRAPPER_H_

#include <string>

#include "zarun/modules/native_object.h"
#include "gin/interceptor.h"

namespace zarun {

class ScriptContext;

typedef int ExceptionCode;

DECLARE_NATIVE_OBJECT(ZarunExceptionWrapper),
    public gin::NamedPropertyInterceptor
    DECLARE_NATIVE_OBJECT_START(ZarunExceptionWrapper)

        public :

    static gin::Handle<ZarunExceptionWrapper> Create(ScriptContext* context,
                                                     int code,
                                                     const char* message);

private:
ZarunExceptionWrapper(ScriptContext* context,
                      unsigned short code,
                      const char* name,
                      const char* message);

public:
v8::Local<v8::Value> GetNamedProperty(v8::Isolate* isolate,
                                      const std::string& property) override;
bool SetNamedProperty(v8::Isolate* isolate,
                      const std::string& property,
                      v8::Local<v8::Value> value) override;

unsigned short code() const {
  return code_;
}
std::string name() const {
  return name_;
}
std::string message() const {
  return message_;
}
std::string syscall() const {
  return syscall_;
}
void set_syscall(const char* syscall) {
  syscall_ = syscall;
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
std::string syscall_;
std::string path_;

DECLARE_NATIVE_OBJECT_END

}  // namespace zarun

#endif  // ZARUN_EXCEPTION_WRAPPER_H_
