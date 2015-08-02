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
#ifndef SOLARUS_ARGUMENTS_H
#define SOLARUS_ARGUMENTS_H

#include "solarus/Common.h"
#include <string>
#include <vector>

namespace Solarus {

/**
 * \brief Stores runtime arguments passed to the program.
 *
 * Arguments may come from the command-line or be set explicitly.
 * Provides easy access to find whether a particular option was passed,
 * and to get the value of options of the form key=value.
 */
class SOLARUS_API Arguments {

  public:

    Arguments();
    Arguments(int argc, char** argv);

    const std::string& get_program_name() const;
    const std::vector<std::string>& get_arguments() const;
    bool has_argument(const std::string& option) const;
    std::string get_argument_value(const std::string& key) const;

    void add_argument(const std::string& argument);
    void add_argument(const std::string& key, const std::string& value);

  private:

    std::string program_name;
    std::vector<std::string> args;

};

}

#endif

