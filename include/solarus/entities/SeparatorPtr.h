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
#ifndef SOLARUS_SEPARATOR_PTR_H
#define SOLARUS_SEPARATOR_PTR_H

#include <memory>

namespace Solarus {

class Separator;

/**
 * \brief Alias for shared_ptr of Separator.
 */
using SeparatorPtr = std::shared_ptr<Separator>;

/**
 * \brief Alias for shared_ptr of const Separator.
 */
using ConstSeparatorPtr = std::shared_ptr<const Separator>;

}

#endif
