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
#ifndef SOLARUS_PICKABLE_ITEM_H
#define SOLARUS_PICKABLE_ITEM_H

#include "Common.h"
#include "entities/Detector.h"
#include "movements/FallingHeight.h"
#include "lowlevel/Rectangle.h"

/**
 * @brief An treasure placed on the ground and that the hero can take.
 */
class PickableItem: public Detector {

  private:

    Treasure *treasure;				/**< the treasure obtained when the player picks this item */

    Sprite *shadow_sprite;			/**< sprite of the shadow (if any) */
    FallingHeight falling_height;		/**< indicates whether the item is falling when it appears (except for a fairy) */
    bool will_disappear;			/**< indicates whether the item will disappear after an amount of time
						 * (only possible for items not saved) */

    // current state
    Rectangle shadow_xy;			/**< coordinates of the shadow (which does not move while the item does) */

    uint32_t appear_date;			/**< date when the item is created */
    uint32_t allow_pick_date;			/**< date when the item can be picked */
    bool can_be_picked;				/**< indicates that the item can be picked now (i.e. allow_picked_date is past) */
    uint32_t blink_date;			/**< date when the item starts blinking */
    uint32_t disappear_date;			/**< date when the item disappears */
    bool is_following_boomerang;		/**< true if this item is currently attached to the boomerang */

    // creation and initialization
    PickableItem(Layer layer, int x, int y, Treasure *treasure);

    void initialize_sprites(void);
    void initialize_movement(void);

    // falling
    bool is_falling(void);

    // item
    void give_item_to_player(void);
    void set_blinking(bool blinking);

  public:

    // creation and destruction
    static PickableItem * create(Game *game, Layer layer, int x, int y,	Treasure *treasure,
	FallingHeight falling_height, bool will_disappear);

    ~PickableItem(void);
    static CreationFunction parse;

    EntityType get_type(void);

    // item state
    void set_suspended(bool suspended);
    void notify_collision(MapEntity *entity_overlapping, CollisionMode collision_mode);
    void update(void);
    void display_on_map(void);
};

#endif

