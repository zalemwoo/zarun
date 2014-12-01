/*
 * environment.h
 *
 */

#ifndef ZARUN_PLATFORM_BASE_ENVIRONMENT_H_
#define ZARUN_PLATFORM_BASE_ENVIRONMENT_H_

#include <string>
#include <vector>

#include "base/macros.h"
#include "base/memory/scoped_ptr.h"

#include "platform/platform_export.h"

namespace base {
class Environment;
}

namespace zarun {
namespace platform {

// chromium's base::Environment wrapper class.
class PLATFORM_EXPORT Environment {
 public:
  Environment();
  ~Environment();

  // Gets an environment variable's value and stores it in |result|.
  // Returns false if the key is unset.
  bool GetVar(const char* variable_name, std::string* result);

  // Syntactic sugar for GetVar(variable_name, NULL);
  bool HasVar(const char* variable_name);

  // Returns true on success, otherwise returns false.
  bool SetVar(const char* variable_name, const std::string& new_value);

  // Returns true on success, otherwise returns false.
  bool UnSetVar(const char* variable_name);

  static std::vector<std::string> GetAll();

 private:
  scoped_ptr<::base::Environment> env_;

  DISALLOW_COPY_AND_ASSIGN(Environment);
};
}
}  // namespace zarun::platform

#endif  // ZARUN_PLATFORM_BASE_ENVIRONMENT_H_
