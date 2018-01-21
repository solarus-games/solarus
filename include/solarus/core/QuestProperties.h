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

#ifndef SOLARUS_QUEST_PROPERTIES_H
#define SOLARUS_QUEST_PROPERTIES_H

#include "solarus/core/Common.h"
#include "solarus/core/Size.h"
#include "solarus/lua/LuaData.h"
#include <iosfwd>
#include <string>

struct lua_State;

namespace Solarus {

class MainLoop;

/**
 * \brief This class stores properties of a quest (the quest.dat file).
 *
 * These properties are general information about a quest.
 * They include the Solarus compatibility version,
 * the writing directory for savegames of this quest,
 * and some metadata describing the quest.
 */
class SOLARUS_API QuestProperties : public LuaData {

  public:

    QuestProperties();

    bool import_from_lua(lua_State* l) override;
    bool export_to_lua(std::ostream& out) const override;

    std::pair<int, int> get_solarus_version_major_minor() const;
    std::string get_solarus_version() const;
    void set_solarus_version(const std::string& solarus_version);
    std::string get_quest_write_dir() const;
    void set_quest_write_dir(const std::string& quest_write_dir);
    std::string get_title() const;
    void set_title(const std::string& title);
    std::string get_short_description() const;
    void set_short_description(const std::string& short_description);
    std::string get_long_description() const;
    void set_long_description(const std::string& long_description);
    std::string get_author() const;
    void set_author(const std::string& author);
    std::string get_quest_version() const;
    void set_quest_version(const std::string& quest_version);
    std::string get_release_date() const;
    void set_release_date(const std::string& release_date);
    std::string get_website() const;
    void set_website(const std::string& website);
    Size get_normal_quest_size() const;
    void set_normal_quest_size(const Size& normal_quest_size);
    Size get_min_quest_size() const;
    void set_min_quest_size(const Size& min_quest_size);
    Size get_max_quest_size() const;
    void set_max_quest_size(const Size& max_quest_size);

  private:

    std::string solarus_version;       /**< Version of Solarus the quest is
                                        * compatible with. */
    std::string quest_write_dir;       /**< Directory where to save files of
                                        * the quest, relative to
                                        * FileTools::get_base_write_dir(). */
    std::string title;                 /**< Title of the quest. */
    std::string short_description;     /**< One-line description of the quest. */
    std::string long_description;      /**< Longer description of the quest. */
    std::string author;                /**< Who created the quest. */
    std::string quest_version;         /**< Version of the quest. */
    std::string release_date;          /**< Date of the last quest release
                                        * (YYYY-MM-DD). */
    std::string website;               /**< URL of the quest website. */
    Size normal_quest_size;            /**< Default quest size. */
    Size min_quest_size;               /**< Minimum quest size. */
    Size max_quest_size;               /**< Maximum quest size. */

};

}

#endif

