/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
 * 
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SOLARUS_STRING_CONCAT_H
#define SOLARUS_STRING_CONCAT_H

#include <sstream>

/**
 * @brief A subclass of std::string that add operators
 * to directly concatenate elements.
 */
class StringConcat: public std::string {

  private:

    std::ostringstream oss;		/**< used to concatenate non string objects to this string */

  public:

    StringConcat& operator<<(const std::string& element);
    template <typename T> StringConcat& operator<<(const T &element);

};

/**
 * @brief Appends a string to this string.
 * @param element the string to append
 */
inline StringConcat& StringConcat::operator<<(const std::string& element) {

  append(element);
  return *this;
}

/**
 * @brief Appends an object to this string.
 * @param element the object to append
 */
template <typename T>
inline StringConcat& StringConcat::operator<<(const T &element) {

  oss.str("");
  oss << element;
  append(oss.str());
  return *this;
}

#endif

