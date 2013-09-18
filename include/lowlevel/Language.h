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

using std::string;

class Language {

  public:
    // Languages.
    static bool has_language(const string& language_code);
    static void set_language(const string& language_code);
    static const string& get_language();
    static const string& get_language_name(const string& language_code);

  private:
    static string language_code;                    /**< Code of the current language (e.g. "en", "fr", etc.). */
};

#endif
