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
#ifndef SOLARUS_BORDER_SET_H
#define SOLARUS_BORDER_SET_H

#include "solarus/core/Common.h"
#include "solarus/entities/BorderKind.h"
#include <string>
#include <map>

namespace Solarus {

/**
 * \brief A set of borders for the autotiler.
 */
class SOLARUS_API BorderSet {

public:

  BorderSet();
  explicit BorderSet(const std::map<BorderKind, std::string>& patterns, bool inner = false);

  const std::map<BorderKind, std::string>& get_patterns() const;
  void set_patterns(const std::map<BorderKind, std::string>& patterns);

  bool has_pattern(BorderKind border_kind) const;
  std::string get_pattern(BorderKind border_kind) const;
  void set_pattern(BorderKind border_kind, const std::string& pattern_id);

  bool is_inner() const;
  void set_inner(bool inner);

private:

  std::map<BorderKind, std::string> patterns;     /**< Ids of the 12 tile patterns defining the border. */
  bool inner;                                     /**< Whether the borders should be placed inside the countours
                                                   * or outside them. */

};

}

#endif
