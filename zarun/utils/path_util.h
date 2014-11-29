/*
 * path_util.h
 *
 */

#ifndef ZARUN_UTILS_PATH_UTIL_H_
#define ZARUN_UTILS_PATH_UTIL_H_

#include <string>

#include "base/files/file_path.h"

namespace zarun {
namespace util {

base::FilePath ExecutionPath();
base::FilePath MakeAbsoluteFilePathRelateToProgram(
    const std::string& relate_path);
base::FilePath MakeAbsoluteFilePathRelateToProgram(
    const base::FilePath& relate_path);
}
}  // namespcae zarun::util

#endif  // ZARUN_UTILS_PATH_UTIL_H_
