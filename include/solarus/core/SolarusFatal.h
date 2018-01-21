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
#ifndef SOLARUS_SOLARUS_FATAL_H
#define SOLARUS_SOLARUS_FATAL_H

#include "solarus/core/Common.h"
#include <exception>
#include <string>

namespace Solarus {

/**
 * \brief Exception representing a fatal error that stops Solarus.
 */
class SOLARUS_API SolarusFatal : public std::exception {

  public:

    explicit SolarusFatal(const std::string& error_message);
    virtual const char* what() const noexcept override;

  private:

    const std::string error_message;
};

}

#endif

