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
#ifndef SOLARUS_DIALOG_RESOURCES_H
#define SOLARUS_DIALOG_RESOURCES_H

#include "solarus/core/Common.h"
#include "solarus/lua/LuaData.h"
#include <map>

namespace Solarus {

/**
 * \brief Stores the properties of a dialog.
 */
class SOLARUS_API DialogData {

  public:

    DialogData();

    const std::string& get_text() const;
    void set_text(const std::string& text);

    const std::map<std::string, std::string>& get_properties() const;
    bool has_property(const std::string& key) const;
    const std::string& get_property(const std::string& key) const;
    void set_property(const std::string& key, const std::string& value);
    bool remove_property(const std::string& key);

  private:

    std::string text;             /**< The whole text of this dialog. */
    std::map<std::string, std::string>
        properties;               /**< Custom properties of this dialog. */
};

/**
 * \brief Stores the content of a dialog data file.
 */
class SOLARUS_API DialogResources : public LuaData {

  public:

    DialogResources();

    void clear();

    const std::map<std::string, DialogData>& get_dialogs() const;
    bool has_dialog(const std::string& dialog_id) const;
    const DialogData& get_dialog(const std::string& dialog_id) const;
    DialogData& get_dialog(const std::string& dialog_id);
    bool add_dialog(const std::string& dialog_id, const DialogData& dialog);
    bool remove_dialog(const std::string& dialog_id);
    bool set_dialog_id(
        const std::string& old_dialog_id, const std::string& new_dialog_id);

    virtual bool import_from_lua(lua_State* l) override;
    virtual bool export_to_lua(std::ostream& out) const override;

  private:

    static int l_dialog(lua_State* l);

    std::map<std::string, DialogData> dialogs;
};

}

#endif
