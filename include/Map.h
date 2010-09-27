/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
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
#ifndef SOLARUS_MAP_H
#define SOLARUS_MAP_H

#include "Common.h"
#include "Transition.h"
#include "entities/Layer.h"
#include "entities/Ground.h"
#include "lowlevel/Rectangle.h"

/**
 * @brief Represents a map where the game can take place.
 *
 * A map is where a game sequence takes place. It contains various information, including:
 * - the map dimensions
 * - the tileset
 * - the tiles and the other entities
 * - the obstacles
 * - the background music
 * - the position of the camera
 */
class Map {

  private:

    friend class MapLoader; // the map loader modifies the private fields of Map

    static MapLoader map_loader;  /**< the map file parser */

    // map properties

    Game *game;                   /**< the game this map was started in */
    MapId id;                     /**< id of the map */

    int width8;                   /**< map width in 8*8 squares (width8 = get_width() / 8) */
    int height8;                  /**< map height in 8*8 squares (height8 = get_height() / 8) */

    Tileset *tileset;             /**< tileset of the map: every tile of this map
				   * is extracted from this tileset. */

    MusicId music_id;             /**< id of the background music of the map: 
				   * can be a valid music, Music::none or Music::unchanged */

    int world;                    /**< the world where this map is:
				   * - 0 if the map is in the outside world
				   * - -1 if the map is in the inside world
				   * - 1 to 20 if the map is in a dungeon */

    int floor;                    /**< The floor where this map is:
				   * - -16 to 15 for a normal floor
				   * - -100 to indicate that there is no floor
				   * - -99 for the unknown floor '?' */

    Rectangle location;           /**< location of the map in its context: the width and height fields
				   * indicate the map size in pixel, and the x and y field indicate the position:
				   * - in the outside world: location of the map's top-left corner
				   *   relative to the whole world map
				   * - in the inside world: location of the map relative to the whole world map
				   * - in a dungeon: location of the map's top-left corner relative to the whole floor */

    int small_keys_variable;      /**< index of the variable of the savegame where the number of small keys
				   * of this map is saved (-1 means that the small keys are not enabled on this map) */

    // screen

    Camera *camera;               /**< determines the visible area of the map */
    Surface *visible_surface;     /**< surface where the map is displayed - this surface is only the visible part
				   * of the map, so the coordinates on this surface are relative to the screen,
				   * not to the map */
    Rectangle clipping_rectangle; /**< when displaying the map, indicates an area of the surface to be restricted to
				    * (usually, the whole map is considered and this rectangle's values are all 0) */

    // map state
    bool started;                         /**< true if this map is the current map */
    std::string destination_point_name;   /**< current destination point on the map,
					   * or "_same" to keep the hero's coordinates,
					   * or "_side0", "_side1", "_side2" or "_side3"
					   * to place the hero on a side of the map */
    MessageId welcome_message_id;         /**< id of the message to show when the hero arrives in this map */

    MapEntities *entities;        /**< the entities on the map */
    bool suspended;               /**< indicates whether the game is suspended */

    MapScript *script;            /**< LUA script of the map */

    void set_suspended(bool suspended);

  public:

    // creation and destruction
    Map(MapId id);
    ~Map();

    // map properties
    MapId get_id();
    Tileset& get_tileset();
    int get_world_number();
    bool is_in_dungeon();
    bool is_in_outside_world();
    int get_floor();
    bool has_floor();
    const Rectangle &get_location();
    int get_small_keys_variable();
    bool has_small_keys();

    int get_width();
    int get_height();
    int get_width8();
    int get_height8();

    // camera
    Surface* get_visible_surface();
    const Rectangle &get_camera_position();
    void move_camera(int x, int y, int speed);
    void restore_camera();
    bool is_camera_fixed_on_hero();
    void set_clipping_rectangle(const Rectangle &clipping_rectangle = Rectangle());

    // loading
    bool is_loaded();
    void load(Game &game);
    void unload();
    Game& get_game();
    Scripts& get_scripts();
    void notify_opening_transition_finished();

    // entities
    MapEntities& get_entities();

    // presence of the hero
    bool is_started();
    void start();
    void leave();
    void set_welcome_message(const std::string &welcome_message_id);

    // current destination point
    void set_destination_point(const std::string &destination_point_name);
    const std::string& get_destination_point_name();
    int get_destination_side();

    // collisions with obstacles (checked before a move)
    bool test_collision_with_border(int x, int y);
    bool test_collision_with_border(const Rectangle &collision_box);
    bool test_collision_with_tiles(Layer layer, int x, int y, MapEntity *entity_to_check);
    bool test_collision_with_entities(Layer layer, const Rectangle &collision_box, MapEntity *entity_to_check);
    bool test_collision_with_obstacles(Layer layer, const Rectangle &collision_box, MapEntity *entity_to_check);
    bool test_collision_with_obstacles(Layer layer, int x, int y, MapEntity *entity_to_check);
    Ground get_tile_ground(Layer layer, int x, int y);
    Ground get_tile_ground(Layer layer, const Rectangle &coordinates);

    // collisions with detectors (checked after a move)
    void check_collision_with_detectors(MapEntity *entity);
    void check_collision_with_detectors(MapEntity *entity, Sprite *sprite);

    // update and display
    void update();
    void check_suspended();
    void display();
    void display_sprite(Sprite *sprite, int x, int y);
};

/**
 * @brief Tests whether a point is outside the map area.
 * @param x x of the point to check
 * @param y y of the point to check
 * @return true if this point is outside the map area
 */
inline bool Map::test_collision_with_border(int x, int y) {

  return (x < 0 || y < 0 || x >= location.get_width() || y >= location.get_height());
}


#endif

