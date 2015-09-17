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
#ifndef SOLARUS_MAP_ENTITIES_H
#define SOLARUS_MAP_ENTITIES_H

#include "solarus/Common.h"
#include "solarus/containers/Quadtree.h"
#include "solarus/entities/Camera.h"
#include "solarus/entities/EntityPtr.h"
#include "solarus/entities/EntityType.h"
#include "solarus/entities/Ground.h"
#include "solarus/entities/TilePtr.h"
#include "solarus/Transition.h"
#include <list>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Solarus {

class Boomerang;
class CrystalBlock;
class Destination;
class Detector;
class Hero;
class Map;
class NonAnimatedRegions;
class Rectangle;
class Separator;
class Stairs;

using EntityTree = Quadtree<EntityPtr>;

/**
 * \brief Manages the whole content of a map.
 *
 * Each element of a map is called an entity and is an instance of
 * a subclass of Entity.
 * This class stores all entities of the current map:
 * the tiles, the hero, the enemies and all other entities.
 */
class SOLARUS_API MapEntities {

  public:

    // creation and destruction
    MapEntities(Game& game, Map& map);

    // get entities
    Hero& get_hero();
    const Camera& get_camera() const;
    Camera& get_camera();
    Ground get_tile_ground(int layer, int x, int y) const;
    const std::list<EntityPtr>& get_entities();
    const std::list<Stairs*>& get_stairs(int layer);
    const std::list<CrystalBlock*>& get_crystal_blocks(int layer);
    const std::list<const Separator*>& get_separators() const;
    Destination* get_default_destination();

    Entity* get_entity(const std::string& name);
    Entity* find_entity(const std::string& name);
    std::list<Entity*> get_entities_with_prefix(const std::string& prefix);
    std::list<Entity*> get_entities_with_prefix(EntityType type, const std::string& prefix);
    bool has_entity_with_prefix(const std::string& prefix) const;

    void get_entities_in_rectangle(const Rectangle& rectangle, std::vector<EntityPtr>& result) const;
    void get_entities_in_rectangle_sorted(const Rectangle& rectangle, std::vector<EntityPtr>& result) const;

    // handle entities
    void add_entity(const EntityPtr& entity);
    void remove_entity(Entity* entity);
    void remove_entity(const std::string& name);
    void remove_entities_with_prefix(const std::string& prefix);
    int get_entity_relative_z_order(const EntityPtr& entity) const;
    void bring_to_front(Entity& entity);
    void bring_to_back(Entity& entity);
    static bool compare_y(const EntityPtr& first, const EntityPtr& second);  // TODO use a function object
    void set_entity_drawn_in_y_order(Entity& entity, bool drawn_in_y_order);
    void set_entity_layer(Entity& entity, int layer);
    void notify_entity_bounding_box_changed(Entity& entity);

    // specific to some entity types
    bool overlaps_raised_blocks(int layer, const Rectangle& rectangle);
    bool is_boomerang_present();
    void remove_boomerang();

    // map events
    void notify_map_started();
    void notify_map_opening_transition_finished();
    void notify_tileset_changed();
    void notify_map_finished();

    // game loop
    void set_suspended(bool suspended);
    void update();
    void draw();

  private:

    friend class MapLoader;            /**< the map loader initializes the private fields of MapEntities */

    /**
     * \brief Internal fast cached information about the entity insertion order.
     */
    class ZCache {

      public:

        ZCache();

        int get_z(const EntityPtr& entity) const;
        void add(const EntityPtr& entity);
        void remove(const EntityPtr& entity);
        void bring_to_front(const EntityPtr& entity);
        void bring_to_back(const EntityPtr& entity);

      private:

        std::unordered_map<EntityPtr, int> z_values;
        int min = 0;
        int max = 0;
    };

    void add_tile(const TilePtr& tile);
    void set_tile_ground(int layer, int x8, int y8, Ground ground);
    void remove_marked_entities();
    void notify_entity_removed(Entity* entity);
    void update_crystal_blocks();

    // map
    Game& game;                                     /**< the game running this map */
    Map& map;                                       /**< the map */
    int map_width8;                                 /**< number of 8x8 squares on a row of the map grid */
    int map_height8;                                /**< number of 8x8 squares on a column of the map grid */

    // tiles
    int tiles_grid_size;                            /**< number of 8x8 squares in the map
                                                     * (tiles_grid_size = map_width8 * map_height8) */
    std::vector<std::vector<Ground>> tiles_ground;  /**< For each layer, list of size tiles_grid_size
                                                     * representing the ground property
                                                     * of each 8x8 square. */
    std::vector<std::unique_ptr<NonAnimatedRegions>>
        non_animated_regions;                       /**< For each layer, all non-animated tiles are managed
                                                     * here for performance. */
    std::vector<std::vector<TilePtr>>
        tiles_in_animated_regions;                  /**< For each layer, animated tiles and tiles overlapping them. */

    // dynamic entities
    Hero& hero;                                     /**< the hero (stored in Game because it is kept when changing maps) */
    std::shared_ptr<Camera>
        camera;                                     /**< The visible area of the map. */

    std::map<std::string, Entity*>
        named_entities;                             /**< Entities identified by a name. */
    std::list<EntityPtr> all_entities;              /**< All map entities except tiles and the hero. */

    EntityTree quadtree;                            /**< All map entities except tiles.
                                                     * Optimized for fast spatial search. */
    std::vector<ZCache> z_caches;                   /**< For each layer, tracks the relative Z order of entities. */

    std::list<Entity*> entities_to_remove;          /**< list of entities that need to be removed right now */

    std::vector<std::list<EntityPtr>>
        entities_drawn_first;                       /**< For each layer, all map entities that are
                                                     * drawn in normal order */

    std::vector<std::list<EntityPtr>>
        entities_drawn_y_order;                     /**< For each layer, all map entities that are drawn in the order
                                                     * defined by their y position, including the hero. */

    Destination* default_destination;               /**< the default destination of this map */

    std::vector<std::list<Stairs*>> stairs;         /**< For each layer, all stairs. */
    std::vector<std::list<CrystalBlock*>>
        crystal_blocks;                             /**< For each layer, all crystal blocks. */
    std::list<const Separator*> separators;         /**< All separators of the map. */

    Boomerang* boomerang;                           /**< The boomerang if present on the map, nullptr otherwise. */

};

/**
 * \brief Returns the ground property of tiles at the specified point.
 *
 * Only static tiles are considered here (not the dynamic entities).
 * Use get_ground() instead to also take into account dynamic entities that
 * may change the ground, like dynamic tiles and destructible entities.
 *
 * This function assumes that the parameters are correct: for performance
 * reasons, no check is done here.
 *
 * \param layer Layer of the point.
 * \param x X coordinate of the point.
 * \param y Y coordinate of the point.
 * \return The ground of the highest tile at this place.
 */
inline Ground MapEntities::get_tile_ground(int layer, int x, int y) const {

  // Warning: this function is called very often so it has been optimized and
  // should remain so.

  // Optimization of: return tiles_ground[layer][(y / 8) * map_width8 + (x / 8)];
  return tiles_ground[layer][(y >> 3) * map_width8 + (x >> 3)];
}

/**
 * \brief Returns the camera of the map.
 * \return The camera.
 */
inline const Camera& MapEntities::get_camera() const {

  return *camera;
}

/**
 * \overload Non-const version.
 */
inline Camera& MapEntities::get_camera() {

  return *camera;
}

}

#endif

