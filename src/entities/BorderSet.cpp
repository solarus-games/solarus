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
#include "solarus/entities/BorderSet.h"

namespace Solarus {

/**
 * \brief Creates an empty border set.
 */
BorderSet::BorderSet() :
  patterns(),
  inner(false) {

}

/**
 * \brief Creates a border set with the given properties.
 * \param patterns The pattern ids to set.
 * \param inner Whether the tiles should be generated inside or outside the countour.
 */
BorderSet::BorderSet(const std::map<BorderKind, std::string>& patterns, bool inner):
  patterns(patterns),
  inner(inner) {

}

/**
 * \brief Returns the patterns ids of this border set.
 * \return The pattern ids.
 */
const std::map<BorderKind, std::string>& BorderSet::get_patterns() const {
  return patterns;
}

/**
 * \brief Sets the patterns ids of this border set.
 * \param patterns The pattern ids.
 */
void BorderSet::set_patterns(const std::map<BorderKind, std::string>& patterns) {

  this->patterns = patterns;
}

/**
 * \brief Returns whether a pattern is defined for the given border.
 * \param border_kind Which border to get.
 * \return \c true if there is a pattern for this border kind.
 */
bool BorderSet::has_pattern(BorderKind border_kind) const {

  return patterns.find(border_kind) != patterns.end();
}

/**
 * \brief Returns the pattern id of the given border.
 * \param border_kind Which border to get.
 * \return The corresponding pattern id, or an empty string to mean no pattern.
 */
std::string BorderSet::get_pattern(BorderKind border_kind) const {

  auto it = patterns.find(border_kind);
  if (it == patterns.end()) {
    return "";
  }
  return it->second;
}

/**
 * \brief Sets the pattern id of the given border.
 * \param border_kind Which border to set.
 * \param pattern_id The corresponding pattern id,
 * or an empty string to mean no pattern.
 */
void BorderSet::set_pattern(BorderKind border_kind, const std::string& pattern_id) {

  if (pattern_id.empty()) {
    patterns.erase(border_kind);
    return;
  }

  patterns[border_kind] = pattern_id;
}

/**
 * \brief Returns whether the borders should be generated inside or outside
 * the countour.
 * \return \c true if these borders should be generated outside the countour.
 */
bool BorderSet::is_inner() const {
  return inner;
}

/**
 * \brief Sets whether the borders should be generated inside or outside
 * the countour.
 * \param inner \c true if these borders should be generated outside the
 * countour.
 */
void BorderSet::set_inner(bool inner) {
  this->inner = inner;
}

}
