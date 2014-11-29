/*
 * file_utils.cc
 *
 */

#include "zarun/utils/file_util.h"

#include "base/logging.h"
#include "base/files/file_util.h"

namespace zarun {
namespace util {

std::string ReadFile(const base::FilePath& path) {
  std::string source;
  if (!base::ReadFileToString(path, &source))
    LOG(FATAL) << "Unable to read " << path.LossyDisplayName();
  return source;
}
}
}  // namespcae zarun::util
