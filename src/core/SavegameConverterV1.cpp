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
#include "solarus/core/InputEvent.h"
#include "solarus/core/QuestFiles.h"
#include "solarus/core/Savegame.h"
#include "solarus/core/SavegameConverterV1.h"
#include <sstream>
#include <cstring>  // memcpy

namespace Solarus {

/**
 * \brief Creates an old savegame converter with a specified existing file name.
 * \param file_name Name of the savegame file (must exist),
 * relative to the savegames directory of the quest.
 */
SavegameConverterV1::SavegameConverterV1(const std::string& file_name) {

  Debug::check_assertion(QuestFiles::data_file_exists(file_name),
      std::string("Cannot convert savegame '") + file_name
      + "': file does not exist"
  );

  // Let's load this obsolete savegame.
  const std::string& buffer = QuestFiles::data_file_read(file_name);
  Debug::check_assertion(buffer.size() == sizeof(SavedData),
      std::string("Cannot read savegame file version 1 '")
      + file_name + "': invalid file size"
  );
  std::memcpy(&saved_data, buffer.data(), sizeof(SavedData));
}

/**
 * \brief Returns a string value saved.
 * \param index index of the value to get, between 0 and 63
 * (see enum StringIndex for their definition).
 * \return The string value saved at this index.
 */
std::string SavegameConverterV1::get_string(int index) {
  return saved_data.strings[index];
}

/**
 * \brief Returns a integer value saved.
 *
 * Values between 0 and 1023 are used by the engine (the C++ code)
 * and values between 1024 and 2047 are available to the map scripts.
 * \param index index of the value to get, between 0 and 2047
 * (see enum IntegerIndex for their definition)
 * \return the integer value saved at this index
 */
uint32_t SavegameConverterV1::get_integer(int index) {
  return saved_data.integers[index];
}

/**
 * \brief Returns a boolean value saved.
 * \param index index of the value to get, between 0 and 32767
 * \return the boolean value saved at this index
 */
bool SavegameConverterV1::get_boolean(int index) {

  uint32_t word = saved_data.booleans[index / 32];
  return ((word >> (index % 32)) & 0x0001) != 0x0000;
}

/**
 * \brief Converts this savegame v1 into a savegame v2.
 * \param savegame_v2 The savegame to fill.
 */
void SavegameConverterV1::convert_to_v2(Savegame& savegame_v2) {

  // 1. Built-in values.
  savegame_v2.set_string(Savegame::KEY_STARTING_POINT, get_string(STARTING_POINT));
  if (!get_string(STARTING_MAP).empty()) {
    savegame_v2.set_string(Savegame::KEY_STARTING_MAP, get_string(STARTING_MAP));
  }
  else {
    // Older v1 savegames used integers to identify maps.
    std::ostringstream oss;
    oss << get_integer(STARTING_MAP_INT);
    savegame_v2.set_string(Savegame::KEY_STARTING_MAP, oss.str());
  }
  savegame_v2.set_integer(Savegame::KEY_CURRENT_LIFE, get_integer(CURRENT_LIFE));
  savegame_v2.set_integer(Savegame::KEY_CURRENT_MONEY, get_integer(CURRENT_MONEY));
  savegame_v2.set_integer(Savegame::KEY_CURRENT_MAGIC, get_integer(CURRENT_MAGIC));
  savegame_v2.set_integer(Savegame::KEY_MAX_LIFE, get_integer(MAX_LIFE));
  savegame_v2.set_integer(Savegame::KEY_MAX_MONEY, get_integer(MAX_MONEY));
  savegame_v2.set_integer(Savegame::KEY_MAX_MAGIC, get_integer(MAX_MAGIC));
  savegame_v2.set_string(Savegame::KEY_ITEM_SLOT_1, get_string(ITEM_SLOT_0));
  savegame_v2.set_string(Savegame::KEY_ITEM_SLOT_2, get_string(ITEM_SLOT_1));
  savegame_v2.set_default_keyboard_controls();  // It would be unsafe to get old SDL_Keycode values.
  savegame_v2.set_string(Savegame::KEY_JOYPAD_ACTION, get_string(JOYPAD_ACTION_KEY));
  savegame_v2.set_string(Savegame::KEY_JOYPAD_ATTACK, get_string(JOYPAD_SWORD_KEY));
  savegame_v2.set_string(Savegame::KEY_JOYPAD_ITEM_1, get_string(JOYPAD_ITEM_1_KEY));
  savegame_v2.set_string(Savegame::KEY_JOYPAD_ITEM_2, get_string(JOYPAD_ITEM_2_KEY));
  savegame_v2.set_string(Savegame::KEY_JOYPAD_PAUSE, get_string(JOYPAD_PAUSE_KEY));
  savegame_v2.set_string(Savegame::KEY_JOYPAD_RIGHT, get_string(JOYPAD_RIGHT_KEY));
  savegame_v2.set_string(Savegame::KEY_JOYPAD_UP, get_string(JOYPAD_UP_KEY));
  savegame_v2.set_string(Savegame::KEY_JOYPAD_LEFT, get_string(JOYPAD_LEFT_KEY));
  savegame_v2.set_string(Savegame::KEY_JOYPAD_DOWN, get_string(JOYPAD_DOWN_KEY));
  savegame_v2.set_integer(Savegame::KEY_ABILITY_TUNIC, get_integer(ABILITY_TUNIC));
  savegame_v2.set_integer(Savegame::KEY_ABILITY_SWORD, get_integer(ABILITY_SWORD));
  savegame_v2.set_integer(Savegame::KEY_ABILITY_SHIELD, get_integer(ABILITY_SHIELD));
  savegame_v2.set_integer(Savegame::KEY_ABILITY_LIFT, get_integer(ABILITY_LIFT));
  savegame_v2.set_integer(Savegame::KEY_ABILITY_SWIM, get_integer(ABILITY_SWIM));
  savegame_v2.set_integer(Savegame::KEY_ABILITY_SWORD_KNOWLEDGE, get_integer(ABILITY_SWORD_KNOWLEDGE));
  savegame_v2.set_integer(Savegame::KEY_ABILITY_DETECT_WEAK_WALLS, get_integer(ABILITY_DETECT_WEAK_WALLS));
  savegame_v2.set_integer(Savegame::KEY_ABILITY_GET_BACK_FROM_DEATH, get_integer(ABILITY_GET_BACK_FROM_DEATH));
  savegame_v2.set_integer(Savegame::KEY_ABILITY_RUN, get_integer(ABILITY_RUN));

  // 2. Values that used to be built-in in v1 and become pure data in v2.
  savegame_v2.set_string("player_name", get_string(PLAYER_NAME));
  savegame_v2.set_integer("pause_last_submenu", get_integer(PAUSE_LAST_SUBMENU) + 1);
  savegame_v2.set_integer("pause_inventory_last_item_index", get_integer(INVENTORY_LAST_ITEM_INDEX) + 1);

  for (int i = 0; i < 40; i++) {
    int index = 200 + i * 10;

    std::ostringstream oss;
    oss << "dungeon_" << (i + 1);
    const std::string& dungeon_number = oss.str();

    // Dungeon finished (integer replaced by a boolean).
    if (get_integer(index) > 0) {
      savegame_v2.set_boolean(dungeon_number + "_finished", true);
    }

    // Got the map? (integer replaced by a boolean).
    ++index;
    if (get_integer(index) > 0) {
      savegame_v2.set_boolean(dungeon_number + "_map", true);
    }

    // Got the compass? (integer replaced by a boolean).
    ++index;
    if (get_integer(index) > 0) {
      savegame_v2.set_boolean(dungeon_number + "_compass", true);
    }

    // Got the big key? (integer replaced by a boolean).
    ++index;
    if (get_integer(index) > 0) {
      savegame_v2.set_boolean(dungeon_number + "_big_key", true);
    }

    // Got the boss key? (integer replaced by a boolean).
    ++index;
    if (get_integer(index) > 0) {
      savegame_v2.set_boolean(dungeon_number + "_boss_key", true);
    }

    // Number of small keys.
    ++index;
    int nb_small_keys = get_integer(index);
    if (nb_small_keys > 0) {
      savegame_v2.set_integer(dungeon_number + "_small_keys", nb_small_keys);
    }
  }

  // 3. Custom values.
  int i;
  for (i = 32; i < 64; i++) {
    const std::string& value = get_string(i);
    if (!value.empty()) {
      std::ostringstream oss;
      oss << "s" << i;
      savegame_v2.set_string(oss.str(), value);
    }
  }

  for (i = 1024; i < 2048; i++) {
    int value = get_integer(i);
    if (value != 0) {
      std::ostringstream oss;
      oss << "i" << i;
      savegame_v2.set_integer(oss.str(), value);
    }
  }

  for (i = 0; i < 32768; i++) {
    bool value = get_boolean(i);
    if (value) {
      std::ostringstream oss;
      oss << "b" << i;
      savegame_v2.set_boolean(oss.str(), value);
    }
  }
}

}

