/*
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

#ifndef SOLARUS_LANGUAGE_H
#define SOLARUS_LANGUAGE_H

#include <string>

namespace Solarus {

/**
 * \brief Provide language management features.
 *
 * This class handles the current language and the list of available languages.
 */
class Language {

  public:

    // Languages.
    static bool has_language(const std::string& language_code);
    static void set_language(const std::string& language_code);
    static const std::string& get_language();
    static std::string get_language_name(const std::string& language_code);

  private:

    static std::string language_code;       /**< Code of the current language (e.g. "en", "fr", etc.). */

};

}

#endif

