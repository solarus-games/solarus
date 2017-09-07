#ifndef SOLARUS_GUI_GUI_COMMON_H
#define SOLARUS_GUI_GUI_COMMON_H

#include "solarus/Common.h"

/**
 * \file gui_common.h
 * \brief This header should be included by each class header file
 * of the Solarus GUI library.
 */

namespace SolarusGui {

/**
 * \brief Returns a const version of the given value.
 *
 * Equivalent to std::as_const (C++17).
 *
 * \brief value Any value.
 * \return The corresponding const value.
 */
template <typename T>
constexpr const T& as_const(T& value) noexcept {
  return value;
}

/**
 * \brief Overload to prevent rvalues.
 */
template <typename T>
void as_const(const T&&) = delete;

}  // namespace SolarusGui

// Windows specific.
#ifndef SOLARUS_GUI_LIBRARY_EXPORT
#  ifdef solarus_gui_lib_EXPORTS  // Define automatically added by cmake.
#    define SOLARUS_GUI_LIBRARY_EXPORT 1
#  else
#    define SOLARUS_GUI_LIBRARY_EXPORT 0
#  endif
#else
#  define SOLARUS_GUI_LIBRARY_EXPORT 0
#endif

/**
 * \def SOLARUS_API
 * \brief Windows DLL import/export specifications for the Solarus library symbols.
 */
#ifdef _WIN32
#  if SOLARUS_GUI_LIBRARY_EXPORT == 1
#    define SOLARUS_GUI_API __declspec(dllexport)
#  else
#    define SOLARUS_GUI_API __declspec(dllimport)
#  endif
#else
#  define SOLARUS_GUI_API
#endif

#endif
