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
#ifndef SOLARUS_ENUM_INFO_H
#define SOLARUS_ENUM_INFO_H

#include "solarus/core/Common.h"
#include <list>
#include <map>
#include <string>

namespace Solarus {

template <typename E>
class SOLARUS_API EnumInfo {

  public:

    using names_type = std::map<E, std::string>;

    static const std::list<E> enums();
    static const std::list<std::string> names();

};

/**
 * Implementers must provide:
 * - static const std::string pretty_name;
 * - static const EnumInfo<E>::names_type names;
 */
template <typename E>
struct EnumInfoTraits;

/**
 * \brief Returns the name corresponding to an enum value.
 * \param value An enum value.
 * \return The corresponding name.
 */
template <typename E>
const std::string& enum_to_name(E value);

/**
 * \brief Returns an enum value from its name.
 * \param name An enum name.
 * \return The corresponding value.
 */
template <typename E>
E name_to_enum(const std::string& name);

/**
 * \brief Returns an enum value from its name.
 * \param name An enum name.
 * \param default_value The default value to return if name is not found.
 * \return The corresponding value if found, otherwise default_value.
 */
template <typename E>
E name_to_enum(const std::string& name, E default_value);

/**
 * \brief Returns an enum value from its name.
 * \param name An enum name.
 * \param default_value The default value to return if name is not found.
 * \param success Set to \c true if the name is found.
 * \return The corresponding value if found, otherwise default_value.
 */
template <typename E>
E name_to_enum(const std::string& name, E default_value, bool& success);

}

#include "EnumInfo.inl"

#endif

