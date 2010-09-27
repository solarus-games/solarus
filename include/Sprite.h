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
#ifndef SOLARUS_SPRITE_H
#define SOLARUS_SPRITE_H

#include "Common.h"
#include <map>

/**
 * @brief Represents an animated sprite.
 *
 * A sprite is represented as a set of animations
 * (i.e. an instance of SpriteAnimationSet),
 * a current animation,
 * a current direction and a current frame.
 * Several sprites can have the same animation set (i.e. they share
 * the same SpriteAnimationSet object).
 *
 * A sprite can be displayed directly on a surface, or it can
 * be attached to a map entity, allowing it to have a position on the map.
 */
class Sprite {

  private:

    // animation set
    static std::map<SpriteAnimationSetId, SpriteAnimationSet*> all_animation_sets;
    const SpriteAnimationSetId animation_set_id;	/**< id of this sprite's animation set */
    SpriteAnimationSet &animation_set;			/**< animation set of this sprite */

    // current state of the sprite

    std::string current_animation_name;			/**< name of the current animation */
    SpriteAnimation *current_animation;			/**< the current animation */
    int current_direction;				/**< current direction of the animation (the first one is number 0);
							 * it can be different from the movement direction
							 * of the entity, because sometimes a sprite can
							 * go backwards. */
    int current_frame;					/**< current frame of the animation (the first one is number 0) */
    bool frame_changed;					/**< indicates that the frame has just changed */

    uint32_t frame_delay;				/**< delay between two frames in milliseconds */
    uint32_t next_frame_date;				/**< date of the next frame */

    bool suspended;					/**< true if the game is suspended */
    bool ignore_suspend;				/**< true to continue playing the animation even when the game is suspended */
    bool paused;					/**< true if the animation is paused */
    bool finished;					/**< true if the animation has been stopped because the last frame is finished */

    // effects

    uint32_t blink_delay;				/**< blink delay of the sprite, or zero if the sprite is not blinking */
    bool blink_is_sprite_visible;			/**< when blinking, true if the sprite is visible or false if it is invisible */
    uint32_t blink_next_change_date;			/**< date of the next change when blinking: visible or not */

    int alpha;						/**< alpha effect applied on the sprite (0: transparent, 255: opaque) */
    uint32_t alpha_next_change_date;			/**< date of the next alpha change when applying a fade-in or fade-out effect */
    int alpha_increment;				/**< increment of the alpha value while fading */
    static Surface *alpha_surface;			/**< an intermediary surface used when blitting with transparency */

    static SpriteAnimationSet& get_animation_set(const SpriteAnimationSetId &id);
    int get_next_frame() const;

  public:

    // initialization
    static void initialize();
    static void quit();

    // creation and destruction
    Sprite(const SpriteAnimationSetId &id);
    ~Sprite();

    void set_map(Map &map);

    // animation set
    const SpriteAnimationSetId& get_animation_set_id() const;
    bool contains(const std::string &s) const;
    SpriteAnimationSet& get_animation_set();
    void enable_pixel_collisions();
    bool are_pixel_collisions_enabled() const;

    // size and origin point
    const Rectangle& get_size() const;
    const Rectangle& get_origin() const;

    // current animation, direction and frame
    const std::string& get_current_animation() const;
    void set_current_animation(const std::string &animation_name);
    int get_current_direction() const;
    void set_current_direction(int current_direction);
    int get_current_frame() const;
    void set_current_frame(int current_frame);

    uint32_t get_frame_delay() const;
    void set_frame_delay(uint32_t frame_delay);

    // animation state
    bool is_animation_started() const;
    void start_animation();
    void restart_animation();
    void stop_animation();

    bool is_suspended() const;
    void set_suspended(bool suspended);
    void set_ignore_suspend(bool ignore_suspend);
    bool is_paused() const;
    void set_paused(bool suspended);
    bool is_animation_looping() const;
    bool is_animation_finished() const;
    bool is_last_frame_reached() const;
    bool has_frame_changed() const;

    // effects
    bool is_blinking() const;
    void set_blinking(uint32_t blink_delay);
    int get_alpha() const;
    void set_alpha(int alpha);
    bool is_fading() const;
    void start_fading(int direction);

    // collisions
    bool test_collision(Sprite *other, int x1, int y1, int x2, int y2);

    // udpate and display
    void update();
    void display(Surface *destination, int x, int y);
};

#endif

