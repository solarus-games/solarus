/*
 * Copyright (C) 2006-2015 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_OUTPUT_H
#define SOLARUS_OUTPUT_H

#include "solarus/Common.h"

namespace Solarus {

class Arguments;

/**
 * \brief Initializes appropriate output facilities.
 *
 * This should be done before any output.
 */
namespace Output {

void SOLARUS_API initialize(const Arguments& args);
void SOLARUS_API quit();

}

}

#endif

