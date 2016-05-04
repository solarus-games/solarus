/*
 * Copyright (C) 2006-2016 Christopho, Solarus - http://www.solarus-games.org
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

#include "solarus/Common.h"
#include "solarus/entities/Camera.h"
#include "solarus/entities/Ground.h"
#include "solarus/entities/Entities.h"
#include "solarus/entities/NonAnimatedRegions.h"
#include "solarus/entities/TilePattern.h"
#include "solarus/entities/Tileset.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/Rectangle.h"
#include "solarus/lowlevel/SurfacePtr.h"
#include "solarus/lua/ExportableToLua.h"
#include "solarus/MapData.h"
#include "solarus/Transition.h"

namespace Solarus {

class Destination;
class InputEvent;
class LuaContext;
class Tileset;
class Sprite;

/**
 * \brief Represents a map where the game can take place.
 *
 * A map is a game scene. It contains various information, including:
 * - its dimensions,
 * - the tileset,
 * - the tiles and the other entities placed on the map,
 * - the ground of each 8x8 square,
 * - the background music.
 */
class SOLARUS_API Map: public ExportableToLua {

  public:

    // creation and destruction
    explicit Map(const std::string& id);

    // map properties
    const std::string& get_id() const;
    Tileset& get_tileset();
    const std::string& get_tileset_id() const;
    void set_tileset(const std::string& tileset_id);
    const std::string& get_music_id() const;
    bool has_world() const;
    const std::string& get_world() const;
    void set_world(const std::string& world);
    bool has_floor() const;
    int get_floor() const;
    void set_floor(int floor);
    const Rectangle& get_location() const;

    Size get_size() const;
    int get_width() const;
    int get_height() const;
    int get_width8() const;
    int get_height8() const;

    int get_min_layer() const;
    int get_max_layer() const;
    bool is_valid_layer(int layer) const;

    // camera
    const CameraPtr& get_camera() const;
    SurfacePtr get_camera_surface();  // TODO remove

    // loading
    bool is_loaded() const;
    void load(Game& game);
    void unload();
    Game& get_game();
    LuaContext& get_lua_context();
    virtual const std::string& get_lua_type_name() const override;

    void notify_opening_transition_finished();

    // entities
    Entities& get_entities();
    const Entities& get_entities() const;

    // presence of the hero
    bool is_started() const;
    void start();
    void leave();

    // current destination point
    void set_destination(const std::string& destination_name);
    const std::string& get_destination_name() const;
    Destination* get_destination();
    int get_destination_side() const;

    // collisions with obstacles (checked before a move)
    bool test_collision_with_border(int x, int y) const;
    bool test_collision_with_border(const Point& point) const;
    bool test_collision_with_border(const Rectangle& collision_box) const;
    bool test_collision_with_ground(
        int layer,
        int x,
        int y,
        const Entity& entity_to_check,
        bool& found_diagonal_wall
    ) const;
    bool test_collision_with_entities(
        int layer,
        const Rectangle& collision_box,
        Entity& entity_to_check
    );
    bool test_collision_with_obstacles(
        int layer,
        const Rectangle& collision_box,
        Entity& entity_to_check
    );
    bool test_collision_with_obstacles(
        int layer,
        int x,
        int y,
        Entity& entity_to_check
    );
    bool test_collision_with_obstacles(
        int layer,
        const Point& point,
        Entity& entity_to_check
    );
    bool has_empty_ground(
        int layer,
        const Rectangle& collision_box
    ) const;

    Ground get_ground(
        int layer,
        int x,
        int y,
        const Entity* entity_to_check
    ) const;
    Ground get_ground(
        int layer,
        const Point& xy,
        const Entity* entity_to_check
    ) const;
    Ground get_ground_from_entity(const Entity& entity, int x, int y) const;
    Ground get_ground_from_entity(const Entity& entity, const Point& xy) const;

    // collisions with detectors (checked after a move)
    void check_collision_with_detectors(Entity& entity);
    void check_collision_with_detectors(Entity& entity, Sprite& sprite);
    void check_collision_from_detector(Entity& detector);
    void check_collision_from_detector(Entity& detector, Sprite& detector_sprite);

    // main loop
    bool notify_input(const InputEvent& event);
    void update();
    bool is_suspended() const;
    void check_suspended();
    void draw();
    void draw_sprite(Sprite& sprite, const Point& xy);
    void draw_sprite(Sprite& sprite, int x, int y);
    void draw_sprite(Sprite& sprite, int x, int y,
        const Rectangle& clipping_area);

  private:

    void set_suspended(bool suspended);
    void build_background_surface();
    void build_foreground_surface();
    void draw_background(const SurfacePtr& dst_surface);
    void draw_foreground(const SurfacePtr& dst_surface);

    // map properties

    Game* game;                   /**< The game this map was started in. */
    std::string id;               /**< Id of this map. */

    int width8;                   /**< Map width in 8x8 squares (width8 = get_width() / 8). */
    int height8;                  /**< Map height in 8x8 squares (height8 = get_height() / 8). */
    int min_layer;                /**< Lowest layer of the map (0 or less). */
    int max_layer;                /**< Highest layer of the map (0 or more). */

    std::string tileset_id;       /**< Id of the current tileset. */
    std::unique_ptr<Tileset>
        tileset;                  /**< Tileset of the map: every tile of this map
                                   * is extracted from this tileset. */

    std::string music_id;         /**< Id of the current music of the map:
                                   * can be a valid music, Music::none or Music::unchanged. */

    std::string world;            /**< Name of the context where this map is. When changing context,
                                   * the savegame starting position is set and crystal switches are reset. */

    int floor;                    /**< The floor where this map is (possibly MapData::NO_FLOOR). */

    Rectangle location;           /**< Location of the map in its context: the width and height fields
                                   * indicate the map size in pixel, and the x and y field indicate the position.
                                   * This is used to correctly scroll between adjacent maps. */

    // Quest Zscreen
    SurfacePtr
        background_surface;       /**< A surface filled with the background color of the tileset. */
    SurfacePtr
        foreground_surface;       /**< A surface with black bars when the map is smaller than the screen. */

    // map state
    bool loaded;                  /**< Whether the loading phase is done. */
    bool started;                 /**< Whether this map is the current map. */
    std::string destination_name; /**< Current destination point on the map,
                                   * or "_same" to keep the hero's coordinates,
                                   * or "_side0", "_side1", "_side2" or "_side3"
                                   * to place the hero on a side of the map,
                                   * or an empty string to use the one saved. */

    std::unique_ptr<Entities>
        entities;                 /**< The entities on the map. */
    bool suspended;               /**< Whether the game is suspended. */
};

/**
 * \brief Tests whether a point is outside the map area.
 * \param x x of the point to check.
 * \param y y of the point to check.
 * \return true if this point is outside the map area.
 */
inline bool Map::test_collision_with_border(int x, int y) const {

  return (x < 0 || y < 0 || x >= location.get_width() || y >= location.get_height());
}

/**
 * \brief Tests whether a point is outside the map area.
 * \param point point to check.
 * \return true if this point is outside the map area.
 */
inline bool Map::test_collision_with_border(const Point& point) const {

  return test_collision_with_border(point.x, point.y);
}

/**
 * \brief Returns the tileset associated to this map.
 * \return The tileset.
 */
inline Tileset& Map::get_tileset() {

  SOLARUS_ASSERT(tileset != nullptr,
      std::string("Missing tileset in map '") + get_id() + "'"
  );
  return *tileset;
}

/**
 * \brief Returns the entities of the map.
 *
 * This function should not be called before the map is loaded into a game.
 *
 * \return The entities of the map.
 */
inline const Entities& Map::get_entities() const {
  return *entities;
}

/**
 * \overload Non-const version.
 */
inline Entities& Map::get_entities() {
  return *entities;
}

/**
 * \brief Returns the camera of the map.
 * \return The camera, or nullptr if there is no camera.
 */
inline const CameraPtr& Map::get_camera() const {

  return get_entities().get_camera();
}

}

#endif

