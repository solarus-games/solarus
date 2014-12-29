/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_MAP_DATA_H
#define SOLARUS_MAP_DATA_H

#include "solarus/Common.h"
#include "solarus/lowlevel/Size.h"
#include "solarus/EntityData.h"
#include <array>
#include <deque>

namespace Solarus {

/**
 * \brief A list of entities on a layer of the map.
 *
 * The entity at index 0 is the most to the back.
 */
using EntityList = std::deque<EntityData>;

/**
 * \brief The layer and index of an entity in the map.
 *
 * This is how entities are identified on a map.
 * There is also an additional index on the name for
 * entities that have a name.
 */
struct EntityIndex {

  public:

    /**
     * \brief Creates an invalid index.
     */
    EntityIndex():
      layer(LAYER_LOW),
      index(-1) {
    }

    /**
     * \brief Creates an index.
     * \param layer The layer.
     * \param index The index in that layer.
     */
    EntityIndex(Layer layer, int index):
      layer(layer),
      index(index) {
    }

    /**
     * \brief Returns whether this is a valid index.
     */
    bool is_valid() const {
      return index != -1;
    }

    Layer layer;     /**< Layer of the entity on the map. */
    int index;       /**< Index of the entity in that layer.
                      * -1 means an invalid index. */

};

/**
 * \brief Stores the contents of a map data file.
 */
class SOLARUS_API MapData : public LuaData {

  public:

    MapData();

    Size get_size() const;
    void set_size(const Size& size);
    Point get_location() const;
    void set_location(const Point& location);
    bool has_world() const;
    const std::string& get_world() const;
    void set_world(const std::string& world);
    bool has_floor() const;
    int get_floor() const;
    void set_floor(int floor);
    const std::string& get_tileset_id() const;
    void set_tileset_id(const std::string& tileset_id);
    const std::string& get_music_id() const;
    void set_music_id(const std::string& music_id);

    int get_num_entities() const;
    int get_num_entities(Layer layer) const;

    void set_entity_layer(const EntityIndex& src_index, Layer dst_layer);
    void bring_entity_to_front(const EntityIndex& index);
    void bring_entity_to_back(const EntityIndex& index);

    const EntityData& get_entity(const EntityIndex& index) const;
    EntityData& get_entity(const EntityIndex& index);
    EntityIndex get_entity_index(const std::string& name) const;
    const EntityData* get_entity_by_name(const std::string& name) const;
    EntityData* get_entity_by_name(const std::string& name);
    bool entity_exists(const std::string& name) const;
    bool set_entity_name(const EntityIndex& index, const std::string& name);

    EntityIndex add_entity(const EntityData& entity);
    void remove_entity(const EntityIndex& index);

    virtual bool import_from_lua(lua_State* l) override;
    virtual bool export_to_lua(std::ostream& out) const override;

    static constexpr int NO_FLOOR = -9999;  /**< Represents a non-existent floor (nil in Lua data files). */

  private:

    Size size;                    /**< Size of the map in pixels. */
    std::string world;            /**< World of the map or an empty string. */
    Point location;               /**< Coordinates of the upper-left corner of the map in its world. */
    int floor;                    /**< Floor of the map or NO_FLOOR. */
    std::string tileset_id;       /**< Tileset to use as skin for the map. */
    std::string music_id;         /**< Background music id or "none" or "same". */

    std::array<EntityList, LAYER_NB>
        entities;                 /**< The entities on each layer. */
    std::map<std::string, EntityIndex>
        named_entities;           /**< Entities indexed by their name. */

};

}

#endif


