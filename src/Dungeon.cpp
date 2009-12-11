/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "SDL_Config/SDL_config_lib.h"
#include "Dungeon.h"
#include "StringResource.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Rectangle.h"

/**
 * Creates the specified dungeon.
 * @param dungeon_number a dungeon number between 1 and 20
 */
Dungeon::Dungeon(int dungeon_number):
  dungeon_number(dungeon_number), chests(NULL), bosses(NULL), boss_floor(-100) {

  load();
}

/**
 * Destructor.
 */
Dungeon::~Dungeon(void) {
  if (chests != NULL) {
    delete[] chests;
    delete[] bosses;
  }
}

/**
 * Returns this dungeon number.
 * @return the dungeon number, between 1 and 20
 */
int Dungeon::get_number(void) {
  return dungeon_number;
}

/**
 * Returns the name of the dungeon, in the current language.
 * The returned name respects the syntax specified by the
 * PauseSubmenuMap::set_caption_text() function.
 * @return the dungeon name
 */
const std::string& Dungeon::get_name(void) {
  return name;
}

/**
 * Loads the dungeon properties from file dungeons.zsd.
 */
void Dungeon::load(void) {

  // get the dungeon name
  std::ostringstream oss;
  oss << "map.caption.dungeon_name_" << dungeon_number;
  this->name = StringResource::get_string(oss.str());

  // parse the dungeon file
  std::string file_name = "maps/dungeons/dungeons.zsd"; 

  // TODO make a lowlevel ini parsing class
  SDL_RWops *rw = FileTools::data_file_open_rw(file_name);
  CFG_File ini;
  if (CFG_OpenFile_RW(rw, &ini) != CFG_OK) {
    DIE("Cannot load the dungeon file '" << file_name << "': " << CFG_GetError());
  }

  // parse the floors (the floors must be before the chests and the bosses)
  oss.str("");
  oss << "dungeon_" << dungeon_number << ".floor_";
  const std::string &floor_prefix = oss.str();
  lowest_floor = 100;
  for (CFG_StartGroupIteration(CFG_SORT_ORIGINAL); !CFG_IsLastGroup(); CFG_SelectNextGroup()) {

    const std::string &group_name = CFG_GetSelectedGroupName();

    // parse the floors
    if (group_name.substr(0, floor_prefix.length()) == floor_prefix) {
      // we found a group describing a floor of to this dungeon

      const std::string &suffix = group_name.substr(floor_prefix.length());

      int floor;
      std::istringstream iss(suffix);
      FileTools::read(iss, floor);

      Rectangle size;
      size.set_width(CFG_ReadInt("width", 0));
      size.set_height(CFG_ReadInt("height", 0));
      floor_sizes.push_back(size);

      if (floor < lowest_floor) {
	lowest_floor = floor;
      }
    }
  }

  // now we now how many floors there are
  int nb_floors = get_nb_floors();
  chests = new std::vector<DungeonElement>[nb_floors];
  bosses = new std::vector<DungeonElement>[nb_floors];

  // parse the rest: chests and bosses
  oss.str("");
  oss << "dungeon_" << dungeon_number << ".map_";
  const std::string &elements_prefix = oss.str();
  for (CFG_StartGroupIteration(CFG_SORT_ORIGINAL); !CFG_IsLastGroup(); CFG_SelectNextGroup()) {

    const std::string &group_name = CFG_GetSelectedGroupName();

    if (group_name.substr(0, elements_prefix.length()) == elements_prefix) {
      // we found a group describing an element in this dungeon

      const std::string &suffix = group_name.substr(elements_prefix.length());

      // is it a chest?
      if (suffix.find("chest") != std::string::npos) {
	DungeonElement chest;
	chest.floor = CFG_ReadInt("floor", 0);
	chest.x = CFG_ReadInt("x", 0);
	chest.y = CFG_ReadInt("y", 0);
	chest.savegame_variable = CFG_ReadInt("save", 0);
	chest.big = CFG_ReadInt("big", 0) != 0;

	chests[chest.floor - lowest_floor].push_back(chest);
      }

      // is it a boss or a miniboss?
      else if (suffix.find("boss") != std::string::npos) {
	DungeonElement boss;
	boss.floor = CFG_ReadInt("floor", 0);
	boss.x = CFG_ReadInt("x", 0);
	boss.y = CFG_ReadInt("y", 0);
	boss.savegame_variable = CFG_ReadInt("save", 0);
	boss.big = CFG_ReadInt("big", 0) != 0;

	if (boss.big) {
	  boss_floor = boss.floor;
	}

	bosses[boss.floor - lowest_floor].push_back(boss);
      }
    }
  }

  // close the file
  FileTools::data_file_close_rw(rw);
  CFG_CloseFile(&ini);
}

/**
 * Returns the number of floors in this dungeon.
 * @return the number of floors
 */
int Dungeon::get_nb_floors(void) {
  return floor_sizes.size();
}

/**
 * Returns the lowest floor in this dungeon.
 * @return the lowest floor
 */
int Dungeon::get_lowest_floor(void) {
  return lowest_floor;
}

/**
 * Returns the highest floor in this dungeon.
 * @return the highest floor
 */
int Dungeon::get_highest_floor(void) {
  return get_lowest_floor() + get_nb_floors() - 1;
}

/**
 * Returns the size of the specified floor.
 * @param floor a floor of this dungeon
 * @return the size of this floor
 */
const Rectangle & Dungeon::get_floor_size(int floor) {
  return floor_sizes[floor - get_lowest_floor()];
}

/**
 * Returns the number of floors that can be displayed for this dungeon.
 * No more that 7 floors can be displayed simultaneously.
 * This function is equivalent to std::min(7, get_nb_floors()).
 * @return the number of floors that can be displayed for this dungeon
 */
int Dungeon::get_nb_floors_displayed(void) {
  return std::min(7, get_nb_floors());
}

/**
 * Returns the highest floor that can be displayed when the specified
 * current floor has to be visible.
 */
int Dungeon::get_highest_floor_displayed(int current_floor) {

  int highest_floor = get_highest_floor();
  int highest_floor_displayed;

  // if there are less that 7 loors, they are all visible
  if (get_nb_floors() <= 7) {
    highest_floor_displayed = highest_floor;
  }

  // otherwise we only display 7 floors including the current one
  else if (current_floor >= highest_floor - 2) {
    highest_floor_displayed = highest_floor;	  
  }
  else if (current_floor <= lowest_floor + 2) {
    highest_floor_displayed = lowest_floor + 6;
  }
  else {
    highest_floor_displayed = current_floor + 3;
  }

  return highest_floor_displayed;
}

/**
 * Returns the floor where the boss of this dungeon is.
 * @return the floor of the boss
 */
int Dungeon::get_boss_floor(void) {
  return boss_floor;
}

/**
 * Returns the information about the bosses and the minibosses
 * of this dungeon that are on the specified floor.
 * @param floor a floor of this dungeon
 * @return the information about the bosses and minibosses of this floor
 */
const std::vector<Dungeon::DungeonElement> Dungeon::get_bosses(int floor) {
  return bosses[floor - get_lowest_floor()];
}

/**
 * Returns the information about the chests
 * of this dungeon that are on the specified floor.
 * @param floor a floor of this dungeon
 * @return the information about the chests of this floor
 */
const std::vector<Dungeon::DungeonElement> Dungeon::get_chests(int floor) {
  return chests[floor - get_lowest_floor()];
}

