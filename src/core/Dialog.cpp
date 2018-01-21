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
#include "solarus/core/Dialog.h"

namespace Solarus {

/**
 * \brief Constructor.
 */
Dialog::Dialog():
  text("") {

}

/**
 * Returns the id of this dialog.
 * \return The id.
 */
const std::string& Dialog::get_id() const {
  return id;
}

/**
 * \brief Sets the id of this dialog.
 * \param id The id.
 */
void Dialog::set_id(const std::string& id) {
  this->id = id;
}

/**
 * \brief Returns the whole text of this dialog.
 *
 * Lines are separated by '\n'.
 *
 * \return the text of this dialog
 */
const std::string& Dialog::get_text() const {
  return text;
}

/**
 * \brief Sets the text of this dialog.
 *
 * Lines must be separated by '\n'. The last line of text may end with '\n'.
 *
 * \param text the text of this dialog
 */
void Dialog::set_text(const std::string& text) {
  this->text = text;
}

/**
 * \brief Returns all custom properties of this dialog.
 * \return The custom properties.
 */
const std::map<std::string, std::string>& Dialog::get_properties() const {
  return properties;
}

/**
 * \brief Returns a property exists in this dialog.
 * \param key Key of the property to check.
 * \return true if a property exists with this key.
 */
bool Dialog::has_property(const std::string& key) const {

  Debug::check_assertion(!key.empty() && key != "text" && key != "dialog_id",
      "Invalid property key for dialog");

  return properties.find(key) != properties.end();
}

/**
 * \brief Returns a custom properties of this dialog.
 * \param key Key of the property to get. It must exist.
 * \return The value of this property.
 */
const std::string& Dialog::get_property(const std::string& key) const {

  Debug::check_assertion(!key.empty() && key != "text" && key != "dialog_id",
      "Invalid property key for dialog");

  const auto it = properties.find(key);

  Debug::check_assertion(it != properties.end(),
      std::string("No such dialog property: '") + key + "'");

  return it->second;
}

/**
 * \brief Sets a custom property of this dialog.
 * \param key Key of the property to set.
 * \param value The value to set.
 */
void Dialog::set_property(const std::string& key, const std::string& value) {

  Debug::check_assertion(!key.empty() && key != "text" && key != "dialog_id",
      "Invalid property key for dialog");

  properties[key] = value;
}

}

