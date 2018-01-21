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
#ifndef SOLARUS_DIALOG_H
#define SOLARUS_DIALOG_H

#include "solarus/core/Common.h"
#include <string>
#include <map>

namespace Solarus {

/**
 * \brief A dialog that can be displayed during the game in the dialog box.
 */
class Dialog {

  public:

    Dialog();

    const std::string& get_id() const;
    void set_id(const std::string& id);

    const std::string& get_text() const;
    void set_text(const std::string& text);

    const std::map<std::string, std::string>& get_properties() const;
    bool has_property(const std::string& key) const;
    const std::string& get_property(const std::string& key) const;
    void set_property(const std::string& key, const std::string& value);

  private:

    std::string id;               /**< Id of this dialog. */
    std::string text;             /**< The whole text of this dialog. */
    std::map<std::string, std::string>
        properties;               /**< Custom properties of this dialog. */
};

}

#endif

