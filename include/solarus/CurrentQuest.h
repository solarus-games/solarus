/*
 * Copyright (C) 2006-2015 Christopho, Solarus - http://www.solarus-games.org
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

#include "solarus/Common.h"
#include "solarus/ResourceType.h"
#include "solarus/QuestResources.h"
#include "solarus/StringResources.h"
#include "solarus/DialogResources.h"
#include "solarus/Dialog.h"
#include <map>
#include <string>

namespace Solarus {

/**
 * \brief Provides access to data of the current quest.
 */
namespace CurrentQuest {

void SOLARUS_API initialize();
void SOLARUS_API quit();

QuestResources& SOLARUS_API get_resources();
bool SOLARUS_API resource_exists(ResourceType resource_type, const std::string& id);
const std::map<std::string, std::string>& SOLARUS_API get_resources(ResourceType resource_type);

bool SOLARUS_API has_language(const std::string& language_code);
void SOLARUS_API set_language(const std::string& language_code);
std::string& SOLARUS_API get_language();
std::string SOLARUS_API get_language_name(const std::string& language_code);

StringResources& SOLARUS_API get_strings();
bool SOLARUS_API string_exists(const std::string& key);
const std::string& SOLARUS_API get_string(const std::string& key);

std::map<std::string, Dialog>& SOLARUS_API get_dialogs();
bool SOLARUS_API dialog_exists(const std::string& dialog_id);
const Dialog& SOLARUS_API get_dialog(const std::string& dialog_id);

}

}

#endif

