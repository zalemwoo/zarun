/*
 * zarun_export.h
 *
 *  Created on: Nov 20, 2014
 *      Author: zalem
 */

#ifndef ZARUN_ZARUN_EXPORT_H_
#define ZARUN_ZARUN_EXPORT_H_

#if defined(COMPONENT_BUILD)
#if defined(WIN32)

#if defined(ZARUN_IMPLEMENTATION)
#define ZARUN_EXPORT __declspec(dllexport)
#else
#define ZARUN_EXPORT __declspec(dllimport)
#endif  // defined(ZARUN_IMPLEMENTATION)

#else  // defined(WIN32)
#if defined(ZARUN_IMPLEMENTATION)
#define ZARUN_EXPORT __attribute__((visibility("default")))
#else
#define ZARUN_EXPORT
#endif  // defined(ZARUN_IMPLEMENTATION)
#endif

#else  // defined(COMPONENT_BUILD)
#define ZARUN_EXPORT
#endif

#endif /* ZARUN_ZARUN_EXPORT_H_ */
