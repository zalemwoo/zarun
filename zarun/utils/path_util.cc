/*
 * path_utils.cc
 *
 */

#include "zarun/utils/path_util.h"

#include <vector>

#include "base/logging.h"
#include "base/strings/string_split.h"
#include "base/command_line.h"
#include "base/files/file_util.h"

namespace zarun {
namespace util {

base::FilePath ExecutionPath() {
  base::FilePath executablePath =
      base::CommandLine::ForCurrentProcess()->GetProgram();
  return base::MakeAbsoluteFilePath(executablePath).DirName();
}

base::FilePath MakeAbsoluteFilePathRelateToProgram(
    const base::FilePath& relate_path) {
  return MakeAbsoluteFilePathRelateToProgram(relate_path.AsUTF8Unsafe());
}

base::FilePath MakeAbsoluteFilePathRelateToProgram(
    const std::string& relate_path) {
  base::FilePath root_path = ExecutionPath();

  std::vector<std::string> components;
  base::SplitString(relate_path, '/', &components);

  base::FilePath path;
  for (size_t i = 0; i < components.size(); ++i) {
    // TODO(abarth): Technically the path components can be UTF-8. We don't
    // handle that case correctly yet.
    path = path.AppendASCII(components[i]);
  }

  return root_path.Append(path);
}
}
}  // namespcae zarun::util
