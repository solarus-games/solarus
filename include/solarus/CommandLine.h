/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_COMMAND_LINE_H
#define SOLARUS_COMMAND_LINE_H

#include "Common.h"
#include <string>
#include <vector>

namespace Solarus {

/**
 * \brief Stores the command-line options passed to the program.
 *
 * Provides easy access to find whether a particular option was passed,
 * and to get the value of options of the form key=value.
 */
class CommandLine {

  public:

    CommandLine(int argc, char** argv);

    const std::string& get_program_name() const;
    const std::vector<std::string>& get_arguments() const;
    bool has_argument(const std::string& option) const;
    std::string get_argument_value(const std::string& key) const;

  private:

    std::string program_name;
    std::vector<std::string> args;

};

}

#endif

