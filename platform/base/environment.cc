/*
 * environment.cc
 *
 */

#include "platform/base/environment.h"

#include "build/build_config.h"
#include "base/environment.h"
#include "base/strings/utf_string_conversions.h"

#if defined(OS_POSIX)
#include <unistd.h>
#if defined(OS_MACOSX)
#include <crt_externs.h>
#define environ (*_NSGetEnviron())
#elif !defined(COMPILER_MSVC)
extern char** environ;
#endif
#endif

using ::base::NativeEnvironmentString;

namespace zarun {
namespace platform {

namespace {
// Parses a null-terminated input string of an environment block. The key is
// placed into the given string, and the total length of the line, including
// the terminating null, is returned.
size_t ParseEnvLine(const NativeEnvironmentString::value_type* input,
                    NativeEnvironmentString* key) {
  // Skip to the equals or end of the string, this is the key.
  size_t cur = 0;
  while (input[cur] && input[cur] != '=')
    cur++;
  *key = NativeEnvironmentString(&input[0], cur);

  // Now just skip to the end of the string.
  while (input[cur])
    cur++;
  return cur + 1;
}

#if defined(OS_POSIX)

std::vector<std::string> GetAllEnvVarNameImpl() {
  std::vector<std::string> envs;
  NativeEnvironmentString env_key;
  int size = 0;
  while (environ[size])
    size++;
  for (int i = 0; i < size; ++i) {
    const char* env = environ[i];
    ParseEnvLine(env, &env_key);
    envs.push_back(env_key);
  }

  return envs;
}

#else  // OS_WIN

std::vector<std::string> GetAllEnvVarNameImpl() {
  std::vector<std::string> envs;
  NativeEnvironmentString env_key;

  NativeEnvironmentString::value_type* environment = ::GetEnvironmentStringsW();
  if (environment == NULL)
    return;  // This should not happen.
  NativeEnvironmentString::value_type* p = environment;
  int i = 0;
  while (*p != NULL) {
    NativeEnvironmentString::value_type* s;
    if (*p == L'=') {
      // If the key starts with '=' it is a hidden environment variable.
      p += wcslen(p) + 1;
      continue;
    } else {
      s = wcschr(p, L'=');
    }
    if (!s) {
      s = p + wcslen(p);
    }

    const size_t two_byte_buffer_len = s - p;

    env_key = NativeEnvironmentString(p, two_byte_buffer_len);
    envs.push_back(::base::WideToUTF8(env_key));

    p = s + wcslen(s) + 1;
  }
  FreeEnvironmentStringsW(environment);

  return envs;
}

#endif

}  // namespace

Environment::Environment() : env_(::base::Environment::Create()) {
}

Environment::~Environment() {
}

bool Environment::GetVar(const char* variable_name, std::string* result) {
  return env_->GetVar(variable_name, result);
}

bool Environment::HasVar(const char* variable_name) {
  return env_->HasVar(variable_name);
}

bool Environment::SetVar(const char* variable_name,
                         const std::string& new_value) {
  return env_->SetVar(variable_name, new_value);
}

bool Environment::UnSetVar(const char* variable_name) {
  return env_->UnSetVar(variable_name);
}

// static
std::vector<std::string> Environment::GetAll() {
  return GetAllEnvVarNameImpl();
}
}
}  // namespace zarun::platform
