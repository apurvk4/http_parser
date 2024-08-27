#pragma once

/**
 * @file API.h
 * @brief define PARSER_EXPORT macro for exporting symbols in shared library for
 both windows and linux
 * @version 1.0.0
 * @date 2024-08-19
 *
 *
 * @section LICENSE
 * GNU General Public License v3.0

*/
#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)
#ifdef HTTP_PARSER_EXPORT
#ifdef __GNUC__
#define PARSER_EXPORT __attribute__((dllexport))
#else
#define PARSER_EXPORT __declspec(dllexport)
#endif
#else
#ifdef __GNUC__
#define PARSER_EXPORT __attribute__((dllimport))
#else
#define PARSER_EXPORT __declspec(dllimport)
#endif
#endif
#else
#if defined(__GNUC__) || defined(__clang__)
#if __GNUC__ >= 4 || __clang_major__ >= 3
#define PARSER_EXPORT __attribute__((visibility("default")))
#else
#define PARSER_EXPORT
#endif
#else
#define PARSER_EXPORT
#endif
#endif
