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
#ifndef ZSDX_HERO_SPRITES_H
#define ZSDX_HERO_SPRITES_H

#include "Common.h"
#include "Ground.h"

/**
 * This class handles the animations of the hero's main sprites: the tunic,
 * the sword, the sword stars, the shield and the ground.
 * It also handles the sprite dependent sounds such as the sword sound.
 */
class HeroSprites {

  private:

    Hero *hero;                    /**< the hero */
    Equipment *equipment;          /**< equipment of the player */

    Sprite *tunic_sprite;          /**< sprite of the current tunic */
    Sprite *sword_sprite;          /**< current sword sprite */
    Sprite *sword_stars_sprite;    /**< stars running along the sword when the sword is loading */
    Sprite *shield_sprite;         /**< current shield sprite */
    Sprite *shadow_sprite;         /**< shadow of the hero, only in state JUMPING (in other states,
				    * the shadow is with the tunic sprite) */
    Sprite *ground_sprite;         /**< ground displayed under the hero (e.g. grass or shallow water) */

    Sound *sword_sound;            /**< sound of the current sword */
    Sound *ground_sound;           /**< sound of the current ground displayed under the hero */

    static const int animation_directions[][2];                 /**< allowed directions of the animation for each
								 * directional key combination */
    static const SpriteAnimationSetId tunic_sprite_ids[];       /**< name of each tunic sprite */
    static const SpriteAnimationSetId sword_sprite_ids[];       /**< name of each sword sprite */
    static const SpriteAnimationSetId sword_stars_sprite_ids[]; /**< name of each sword stars sprite */
    static const SpriteAnimationSetId shield_sprite_ids[];      /**< name of each shield sprite */
    static const SpriteAnimationSetId ground_sprite_ids[];      /**< name of each ground sprite */
    static const SoundId sword_sound_ids[];                     /**< name of each sword sound */
    static const SoundId ground_sound_ids[];                    /**< name of each ground sound */

    int animation_direction_saved;  /**< direction of the hero's sprites, saved before
				     * showing a sprite animation having only one direction */
    uint32_t when_suspended;         /**< date when the game was suspended */
    uint32_t end_blink_date;         /**< when the hero's sprites stop blinking */

    bool walking;                  /**< stopped or walking? (used in states FREE, PUSHING and CARRYING) */

  public:

    HeroSprites(Hero *hero, Equipment *equipment);
    ~HeroSprites(void);

    void update(void);
    void display_on_map(void);
    void set_suspended(bool suspended);
    void rebuild_equipment(void);

    void stop_displaying_sword(void);
    void stop_displaying_shield(void);
    void blink(void);
    void stop_blinking(void);
    bool is_blinking(void);
    bool is_walking(void);

    int get_animation_direction(uint32_t direction_mask);
    int get_animation_direction(void);
    void set_animation_direction(int direction);
    void restart_animation(void);
    bool is_animation_finished(void);
    int get_current_frame(void);
    void save_animation_direction(void);
    void restore_animation_direction(void);

    void set_animation_stopped(void);
    void set_animation_walking(void);
    void set_animation_sword(void);
    void play_sword_sound(void);
    void set_animation_sword_tapping(void);
    void set_animation_spin_attack(void);
    void set_animation_grabbing(void);
    void set_animation_pulling(void);
    void set_animation_pushing(void);
    void set_animation_lifting(void);
    void set_animation_jumping(void);
    void set_animation_hurt(void);
    void set_animation_plunging(void);
    void set_animation_falling(void);
    void set_animation_boomerang(void);
    void set_animation_brandish(void);
    void set_animation_victory(void);

    bool is_visible(void);
    bool is_sword_visible(void);
    bool is_sword_stars_visible(void);
    bool is_shield_visible(void);
    bool is_shadow_visible(void);

    void create_ground(Ground grond);
    void destroy_ground(void);
    void play_ground_sound(void);
};

#endif

