/* -------------------------------------------------------------------------- */
#ifndef AEGIS_CONFIG_H
#define AEGIS_CONFIG_H
/* -------------------------------------------------------------------------- */
/* ---------------------------- Define AEGIS version ----------------------- */
/* -------------------------------------------------------------------------- */
// clang-format off

#define AEGIS_VERSION_MAJOR 0
#define AEGIS_VERSION_MINOR 1
#define AEGIS_VERSION_PATCH 0
#define AEGIS_VERSION 0.1.0
#define AEGIS_VERSION_STR "0.1.0"

// clang-format on
/* -------------------------------------------------------------------------- */
/* ----------------------- Identify the operating system -------------------- */
/* -------------------------------------------------------------------------- */
#if defined(_WIN32)
#define AEGIS_OS_WINDOWS
#elif defined(__linux__)
#define AEGIS_OS_LINUX
#else
#error This operating system is not supported by AEGIS library
#endif
/* -------------------------------------------------------------------------- */
/* ------------------------ Define a portable debug macro ------------------- */
/* -------------------------------------------------------------------------- */
#if !defined(NDEBUG)
#define AEGIS_DEBUG
#endif
/* -------------------------------------------------------------------------- */
/* ----------------------- Define a export/import dll macro ----------------- */
/* -------------------------------------------------------------------------- */
#if !defined(AEGIS_STATIC)

#if defined(AEGIS_OS_WINDOWS)

#define AEGIS_API_EXPORT __declspec(dllexport)
#define AEGIS_API_IMPORT __declspec(dllimport)

#ifdef _MSC_VER

#pragma warning(disable : 4251)

#endif

#else

#define AEGIS_API_EXPORT __attribute__((__visibility__("default")))
#define AEGIS_API_IMPORT __attribute__((__visibility__("default")))

#endif

#else

#define AEGIS_API_EXPORT
#define AEGIS_API_IMPORT

#endif
/* -------------------------------------------------------------------------- */
/* -------------------- Define helpers to set items as deprecated ----------- */
/* -------------------------------------------------------------------------- */
#if !defined(AEGIS_DEPRECATED_WARNINGS)
#define AEGIS_DEPRECATED [[deprecated]]
#else
#define AEGIS_DEPRECATED
#endif
/* -------------------------------------------------------------------------- */
#endif  // AEGIS_CONFIG_H
/* -------------------------------------------------------------------------- */
