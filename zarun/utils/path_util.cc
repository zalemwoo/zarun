/*
 * path_utils.cc
 *
 */

#include "zarun/utils/path_util.h"

#include <vector>

#include "base/logging.h"
#include "base/strings/string_split.h"
#include "base/command_line.h"
#include "base/path_service.h"
#include "base/files/file_util.h"

namespace zarun {
namespace util {

base::FilePath ExecutionPath() {
  base::FilePath path;
  if (!PathService::Get(base::DIR_EXE, &path))
    return base::FilePath();
  return path;
}

base::FilePath MakeAbsoluteFilePathRelateToProgram(
    const base::FilePath& relate_path) {
  return MakeAbsoluteFilePathRelateToProgram(relate_path.AsUTF8Unsafe());
}

base::FilePath MakeAbsoluteFilePathRelateToProgram(
    const std::string& relate_path) {
  base::FilePath full_path = ExecutionPath();
  std::vector<std::string> components;
  base::SplitString(relate_path, '/', &components);
  for (size_t i = 0; i < components.size(); ++i) {
    // TODO(abarth): Technically the path components can be UTF-8. We don't
    // handle that case correctly yet.
    if (!components[i].empty())
      full_path = full_path.AppendASCII(components[i]);
  }
  return full_path;
}
}
}  // namespcae zarun::util
