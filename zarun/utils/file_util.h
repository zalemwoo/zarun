/*
 * file_util.h
 *
 */

#ifndef ZARUN_UTILS_FILE_UTIL_H_
#define ZARUN_UTILS_FILE_UTIL_H_

#include "base/files/file_path.h"

namespace zarun {
namespace util {

std::string ReadFile(const base::FilePath& path);
}
}  // namespcae zarun::util

#endif  // ZARUN_UTILS_FILE_UTIL_H_
