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
#ifndef SOLARUS_CURRENT_QUEST_H
#define SOLARUS_CURRENT_QUEST_H

#include "solarus/core/Common.h"
#include "solarus/core/Dialog.h"
#include "solarus/core/ResourceType.h"
#include <map>
#include <string>

namespace Solarus {

class DialogResources;
class QuestProperties;
class QuestDatabase;
class StringResources;

/**
 * \brief Provides access to resources and properties of the current quest.
 */
namespace CurrentQuest {

SOLARUS_API void initialize();
SOLARUS_API void quit();
SOLARUS_API bool is_initialized();

SOLARUS_API QuestProperties& get_properties();

SOLARUS_API QuestDatabase& get_database();
SOLARUS_API bool resource_exists(ResourceType resource_type, const std::string& id);
SOLARUS_API const std::map<std::string, std::string>& get_resources(ResourceType resource_type);

SOLARUS_API bool has_language(const std::string& language_code);
SOLARUS_API void set_language(const std::string& language_code);
SOLARUS_API std::string& get_language();
SOLARUS_API std::string get_language_name(const std::string& language_code);

SOLARUS_API StringResources& get_strings();
SOLARUS_API bool string_exists(const std::string& key);
SOLARUS_API const std::string& get_string(const std::string& key);

SOLARUS_API std::map<std::string, Dialog>& get_dialogs();
SOLARUS_API bool dialog_exists(const std::string& dialog_id);
SOLARUS_API const Dialog& get_dialog(const std::string& dialog_id);

SOLARUS_API bool is_format_at_least(const std::pair<int, int>& version);
SOLARUS_API bool is_format_at_most(const std::pair<int, int>& version);

}

}

#endif
