/*
 * platform_export.h
 *
 */

#ifndef PLATFORM_PLATFORM_EXPORT_H_
#define PLATFORM_PLATFORM_EXPORT_H_

#if defined(COMPONENT_BUILD)
#if defined(WIN32)

#if defined(ZARUN_IMPLEMENTATION)
#define PLATFORM_EXPORT __declspec(dllexport)
#else
#define PLATFORM_EXPORT __declspec(dllimport)
#endif  // defined(ZARUN_IMPLEMENTATION)

#else  // defined(WIN32)
#if defined(ZARUN_IMPLEMENTATION)
#define PLATFORM_EXPORT __attribute__((visibility("default")))
#else
#define PLATFORM_EXPORT
#endif  // defined(ZARUN_IMPLEMENTATION)
#endif

#else  // defined(COMPONENT_BUILD)
#define PLATFORM_EXPORT
#endif

#endif  // PLATFORM_PLATFORM_EXPORT_H_
