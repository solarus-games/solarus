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
#include "solarus/Language.h"
#include "solarus/DialogResource.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/StringResource.h"
#include "solarus/CurrentQuestResources.h"

namespace Solarus {

std::string Language::language_code;

/**
 * \brief Returns whether a language exists for this quest.
 * \param language_code Code of the language to test.
 * \return \c true if this language exists.
 */
bool Language::has_language(const std::string& language_code) {

  return CurrentQuestResources::exists(ResourceType::LANGUAGE, language_code);
}

/**
 * \brief Sets the current language.
 *
 * The language-specific data will be loaded from the directory of this language.
 * This function must be called before the first language-specific file is loaded.
 *
 * \param language_code Code of the language to set.
 */
void Language::set_language(const std::string& language_code) {

  Debug::check_assertion(has_language(language_code),
      std::string("No such language: '") + language_code + "'");

  Language::language_code = language_code;
  StringResource::initialize();
  DialogResource::initialize();
}

/**
 * \brief Returns the current language.
 *
 * The language-specific data are be loaded from the directory of this language.
 *
 * \return code of the language, or an empty string if no language is set
 */
const std::string& Language::get_language() {
  return language_code;
}

/**
 * \brief Returns the user-friendly name of a language for this quest.
 * \param language_code Code of a language.
 * \return Name of this language of an empty string.
 */
std::string Language::get_language_name(
    const std::string& language_code) {

  const std::map<std::string, std::string>& languages =
      CurrentQuestResources::get_elements(ResourceType::LANGUAGE);

  const auto& it = languages.find(language_code);
  if (it != languages.end()) {
    return it->second;
  }

  return "";
}

}

