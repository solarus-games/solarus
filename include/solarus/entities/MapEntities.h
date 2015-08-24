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
#include "solarus/entities/EntityType.h"
#include "solarus/entities/Ground.h"
#include "solarus/entities/Layer.h"
#include "solarus/entities/MapEntityPtr.h"
#include "solarus/entities/TilePtr.h"
#include "solarus/Transition.h"
#include <list>
#include <map>
#include <memory>
#include <string>
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
    Ground get_tile_ground(Layer layer, int x, int y) const;
    const std::list<EntityPtr>& get_entities();
    const std::list<Entity*>& get_obstacle_entities(Layer layer);
    const std::list<Entity*>& get_ground_observers(Layer layer);
    const std::list<Entity*>& get_ground_modifiers(Layer layer);
    const std::list<Detector*>& get_detectors();
    const std::list<Stairs*>& get_stairs(Layer layer);
    const std::list<CrystalBlock*>& get_crystal_blocks(Layer layer);
    const std::list<const Separator*>& get_separators() const;
    Destination* get_default_destination();

    Entity* get_entity(const std::string& name);
    Entity* find_entity(const std::string& name);
    std::list<Entity*> get_entities_with_prefix(const std::string& prefix);
    std::list<Entity*> get_entities_with_prefix(EntityType type, const std::string& prefix);
    bool has_entity_with_prefix(const std::string& prefix) const;

    void get_entities_in_rectangle(const Rectangle& rectangle, std::vector<EntityPtr>& result) const;

    // handle entities
    void add_entity(const EntityPtr& entity);
    void remove_entity(Entity* entity);
    void remove_entity(const std::string& name);
    void remove_entities_with_prefix(const std::string& prefix);
    void bring_to_front(Entity& entity);
    void bring_to_back(Entity& entity);
    static bool compare_y(Entity* first, Entity* second);
    void set_entity_drawn_in_y_order(Entity& entity, bool drawn_in_y_order);
    void set_entity_layer(Entity& entity, Layer layer);
    void notify_entity_bounding_box_changed(Entity& entity, const Rectangle& bounding_box);
    void notify_entity_ground_observer_changed(Entity& entity);
    void notify_entity_ground_modifier_changed(Entity& entity);

    // specific to some entity types
    bool overlaps_raised_blocks(Layer layer, const Rectangle& rectangle);
    bool is_boomerang_present();
    void remove_boomerang();
    void remove_arrows();

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

    void add_tile(const TilePtr& tile);
    void set_tile_ground(Layer layer, int x8, int y8, Ground ground);
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
    std::vector<Ground> tiles_ground[LAYER_NB];     /**< array of size tiles_grid_size representing the ground property
                                                     * of each 8x8 square. */
    std::unique_ptr<NonAnimatedRegions>
        non_animated_regions[LAYER_NB];             /**< All non-animated tiles are managed here for performance. */
    std::vector<TilePtr>
        tiles_in_animated_regions[LAYER_NB];        /**< animated tiles and tiles overlapping them */

    // dynamic entities
    Hero& hero;                                     /**< the hero (stored in Game because it is kept when changing maps) */

    std::map<std::string, Entity*>
      named_entities;                               /**< entities identified by a name */
    std::list<EntityPtr> all_entities;              /**< all map entities except the tiles and the hero;
                                                     * this vector is used to delete the entities
                                                     * when the map is unloaded */

    EntityTree quadtree;                            /**< All map entities except tiles.
                                                     * Optimized for fast spatial search. */
    std::list<Entity*> entities_to_remove;          /**< list of entities that need to be removed right now */

    std::list<Entity*>
      entities_drawn_first[LAYER_NB];               /**< all map entities that are drawn in the normal order */

    std::list<Entity*>
      entities_drawn_y_order[LAYER_NB];             /**< all map entities that are drawn in the order
                                                     * defined by their y position, including the hero */

    std::list<Detector*> detectors;                 /**< all entities able to detect other entities
                                                     * on this map.
                                                     * TODO store them by layer like obstacle_entities
                                                     * but take care of has_layer_independent_collisions() */
    std::list<Entity*>
      ground_observers[LAYER_NB];                   /**< all dynamic entities sensible to the ground
                                                     * below them */
    std::list<Entity*>
      ground_modifiers[LAYER_NB];                   /**< all dynamic entities that may change the ground of
                                                     * the map where they are placed */
    Destination* default_destination;               /**< the default destination of this map */

    std::list<Entity*>
      obstacle_entities[LAYER_NB];                  /**< all entities that might be obstacle for other
                                                     * entities on this map, including the hero */

    std::list<Stairs*> stairs[LAYER_NB];            /**< all stairs of the map */
    std::list<CrystalBlock*>
      crystal_blocks[LAYER_NB];                     /**< all crystal blocks of the map */
    std::list<const Separator*> separators;         /**< all separators of the map */

    Boomerang* boomerang;                           /**< the boomerang if present on the map, nullptr otherwise */

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
inline Ground MapEntities::get_tile_ground(Layer layer, int x, int y) const {

  // Warning: this function is called very often so it has been optimized and
  // should remain so.

  // Optimization of: return tiles_ground[layer][(y / 8) * map_width8 + (x / 8)];
  return tiles_ground[layer][(y >> 3) * map_width8 + (x >> 3)];
}

}

#endif

