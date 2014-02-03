/**
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
#ifndef SOLARUS_HERO_SPRITES_H
#define SOLARUS_HERO_SPRITES_H

#include "Common.h"
#include "entities/Ground.h"
#include "lowlevel/Rectangle.h"

namespace solarus {

/**
 * \brief Manages the animations of the hero's main sprites.
 *
 * This class handles the animations of the hero's main sprites: the tunic,
 * the sword, the sword stars, the shield, the shadow and the ground.
 * It also handles the sprite-dependent sounds such as the sword sound.
 *
 * This class does not know anything about the hero's internal state:
 * it is the role of the state to call this class to display the appropriate animation.
 */
class HeroSprites {

  public:

    HeroSprites(Hero& hero, Equipment& equipment);
    ~HeroSprites();

    void update();
    void draw_on_map();
    void set_suspended(bool suspended);
    void rebuild_equipment();
    void notify_map_started();
    void notify_tileset_changed();

    void blink(uint32_t duration);
    void stop_blinking();
    bool is_blinking() const;
    bool is_walking() const;
    void set_clipping_rectangle(
        const Rectangle& clipping_rectangle = Rectangle());

    int get_animation_direction(
        int keys_direction, int real_movement_direction) const;
    int get_animation_direction() const;
    int get_animation_direction8() const;
    void set_animation_direction(int direction);
    void set_animation_direction8(int direction);
    void restart_animation();
    bool is_animation_finished() const;
    int get_current_frame() const;
    void save_animation_direction();
    void restore_animation_direction();
    void set_ignore_suspend(bool ignore_suspend);

    const std::string& get_tunic_animation() const;

    void set_animation_stopped_common();
    void set_animation_stopped_normal();
    void set_animation_stopped_sword_loading();
    void set_animation_stopped_carrying();
    void set_animation_stopped_swimming();
    void set_animation_walking_common();
    void set_animation_walking_normal();
    void set_animation_walking_sword_loading();
    void set_animation_walking_carrying();
    void set_animation_swimming_slow();
    void set_animation_swimming_fast();
    void set_animation_walking_diagonal(int direction8);
    void set_animation_sword();
    void play_sword_sound();
    void set_animation_sword_tapping();
    void set_animation_spin_attack();
    void set_animation_super_spin_attack();
    void set_animation_grabbing();
    void set_animation_pulling();
    void set_animation_pushing();
    void set_animation_lifting();
    void set_animation_jumping();
    void set_animation_hurt();
    void set_animation_falling();
    void set_animation_brandish();
    void set_animation_victory();
    void set_animation_prepare_running();
    void set_animation_running();
    void set_animation_boomerang(const std::string& tunic_preparing_animation);
    void set_animation(const std::string& animation);
    void set_animation(const std::string& animation, int callback_ref);

    void create_ground(Ground grond);
    void destroy_ground();
    void play_ground_sound();

    void set_lifted_item(CarriedItem* lifted_item);

  private:

    bool is_visible() const;
    bool is_sword_visible() const;
    bool is_sword_stars_visible() const;
    bool is_shield_visible() const;
    bool is_trail_visible() const;
    bool is_ground_visible() const;

    void stop_displaying_sword();
    void stop_displaying_sword_stars();
    void stop_displaying_shield();
    void stop_displaying_trail();

    void set_tunic_animation(const std::string& animation);
    void set_tunic_animation(const std::string& animation, int callback_ref);

    LuaContext& get_lua_context();

    Hero& hero;                             /**< the hero */
    Equipment& equipment;                   /**< equipment of the player */

    Sprite* tunic_sprite;                   /**< sprite of the current tunic */
    Sprite* sword_sprite;                   /**< current sword sprite */
    Sprite* sword_stars_sprite;             /**< stars running along the sword when the sword is loading */
    Sprite* shield_sprite;                  /**< current shield sprite */
    Sprite* shadow_sprite;                  /**< shadow of the hero, only in specific states (most of the time
                                             * the shadow is with the tunic sprite) */
    Sprite* ground_sprite;                  /**< ground displayed under the hero (e.g. grass or shallow water) */
    Sprite* trail_sprite;                   /**< trail of dust that the hero lets behind him (e.g. when running) */

    std::string sword_sound_id;             /**< sound id of the current sword */
    std::string ground_sound_id;            /**< sound id of the current ground displayed under the hero */

    static const int
        animation_directions[8][2];         /**< possible directions of the animation for each movement direction */

    int animation_direction_saved;          /**< direction of the hero's sprites, saved before
                                              * showing a sprite animation having only one direction */
    uint32_t when_suspended;                /**< date when the game was suspended */

    bool blinking;                          /**< Whether the hero's sprites are blinking. */
    uint32_t end_blink_date;                /**< When the hero's sprites stop blinking.
                                             * 0 means infinite. */

    bool walking;                           /**< stopped or walking? */

    Rectangle clipping_rectangle;           /**< when drawing the sprites onto a map, indicates an area of the map to be restricted to
                                             * (usually, the whole map is considered and this rectangle's values are all 0) */

    CarriedItem* lifted_item;               /**< if not NULL, an item to display above the hero */

    int animation_callback_ref;             /**< Lua ref of a function to call when a custom animation ends. */
};

}

#endif

