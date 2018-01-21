/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/core/Debug.h"
#include "solarus/core/String.h"
#include <algorithm>

namespace Solarus {

template <typename E>
const std::list<E> EnumInfo<E>::enums() {
  std::list<E> enums;

  std::transform(EnumInfoTraits<E>::names.begin(), EnumInfoTraits<E>::names.end(), std::back_inserter(enums),
      [] (typename decltype(EnumInfoTraits<E>::names)::const_reference val) {
        return val.first;
      }
  );
  return enums;
}

template <typename E>
const std::list<std::string> EnumInfo<E>::names() {
  std::list<std::string> names;

  std::transform(EnumInfoTraits<E>::names.begin(), EnumInfoTraits<E>::names.end(), std::back_inserter(enums),
      [] (typename decltype(EnumInfoTraits<E>::names)::const_reference val) {
        return val.second;
      }
  );
  return names;
}

template <typename E>
const std::string& enum_to_name(E value) {

  const auto& it = EnumInfoTraits<E>::names.find(value);
  if (it == EnumInfoTraits<E>::names.end()) {
    Debug::die(std::string("Invalid ") + EnumInfoTraits<E>::pretty_name +
               " number: " + String::to_string(static_cast<int>(value)));
  }

  return it->second;
}

template <typename E>
E name_to_enum(const std::string& name) {

  for (const auto& kvp : EnumInfoTraits<E>::names) {
    if (kvp.second == name) {
      return kvp.first;
    }
  }

  Debug::die(std::string("Invalid ") + EnumInfoTraits<E>::pretty_name + " name: '" + name + "'");
}

template <typename E>
E name_to_enum(const std::string& name, E default_value) {

  bool success = false;
  return name_to_enum(name, default_value, success);
}

template <typename E>
E name_to_enum(const std::string& name, E default_value, bool& success) {

  for (const auto& kvp : EnumInfoTraits<E>::names) {
    if (kvp.second == name) {
      success = true;
      return kvp.first;
    }
  }

  success = false;
  return default_value;
}

}  // namespace Solarus

