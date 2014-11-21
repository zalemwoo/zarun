/*
 * zarun_version.h
 *
 *  Created on: Nov 21, 2014
 *      Author: zalem
 */

#ifndef ZARUN_ZARUN_VERSION_H_
#define ZARUN_ZARUN_VERSION_H_

#define ZARUN_VERSION_MAJOR 0
#define ZARUN_VERSION_MINOR 1
#define ZARUN_VERSIN_PATCH 0

#ifndef ZARUN_STRINGIFY
#define ZARUN_STRINGIFY(n) ZARUN_STRINGIFY_HELPER(n)
#define ZARUN_STRINGIFY_HELPER(n) #n
#endif

#define ZARUN_STRING_JOIN_THREE(f, s, t, split) #f split #s split #t
#define ZARUN_VERSION_FULL_(major, minor, patch) \
  ZARUN_STRING_JOIN_THREE(major, minor, patch, ".")
#define ZARUN_VERSION_FULL                                      \
  ZARUN_VERSION_FULL_(ZARUN_VERSION_MAJOR, ZARUN_VERSION_MINOR, \
                      ZARUN_VERSIN_PATCH)

#endif /* ZARUN_ZARUN_VERSION_H_ */
