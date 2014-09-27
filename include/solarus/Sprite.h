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
#ifndef SOLARUS_SPRITE_H
#define SOLARUS_SPRITE_H

#include "Common.h"
#include "Drawable.h"
#include "SpritePtr.h"
#include <map>

namespace Solarus {

class LuaContext;
class Size;
class SpriteAnimation;
class SpriteAnimationSet;
class Tileset;

/**
 * \brief Represents an animated sprite.
 *
 * A sprite is represented as a set of animations
 * (i.e. an instance of SpriteAnimationSet),
 * a current animation,
 * a current direction and a current frame.
 * Several sprites can have the same animation set (i.e. they share
 * the same SpriteAnimationSet object).
 *
 * A sprite can be drawn directly on a surface, or it can
 * be attached to a map entity.
 */
class Sprite: public Drawable {

  public:

    // initialization
    static void initialize();
    static void quit();

    // creation and destruction
    Sprite(const std::string& id);

    void set_tileset(Tileset& tileset);

    // animation set
    const std::string& get_animation_set_id() const;
    const SpriteAnimationSet& get_animation_set() const;
    void enable_pixel_collisions();
    bool are_pixel_collisions_enabled() const;

    // size and origin point
    Size get_size() const;
    const Size& get_max_size() const;
    const Point& get_origin() const;

    // animation state
    const std::string& get_current_animation() const;
    void set_current_animation(const std::string& animation_name);
    bool has_animation(const std::string& animation_name) const;
    int get_current_direction() const;
    int get_nb_directions() const;
    void set_current_direction(int current_direction);
    int get_nb_frames() const;
    int get_current_frame() const;
    void set_current_frame(int current_frame, bool notify_script = true);
    const Rectangle& get_current_frame_rectangle() const;
    uint32_t get_frame_delay() const;
    void set_frame_delay(uint32_t frame_delay);
    void set_synchronized_to(const SpritePtr& other);

    bool is_animation_started() const;
    void start_animation();
    void restart_animation();
    void stop_animation();

    virtual void set_suspended(bool suspended) override;
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

    // collisions
    bool test_collision(const Sprite& other, int x1, int y1, int x2, int y2) const;

    // update and draw
    virtual void update() override;
    virtual void raw_draw(Surface& dst_surface, const Point& dst_position) override;
    virtual void raw_draw_region(const Rectangle& region,
        Surface& dst_surface, const Point& dst_position) override;
    virtual void draw_transition(Transition& transition);
    virtual Surface& get_transition_surface() override;

    LuaContext* get_lua_context() const;
    void set_lua_context(LuaContext* lua_context);
    virtual const std::string& get_lua_type_name() const override;

  private:

    static SpriteAnimationSet& get_animation_set(const std::string& id);
    int get_next_frame() const;
    Surface& get_intermediate_surface() const ;
    void set_frame_changed(bool frame_changed);

    LuaContext* lua_context;           /**< The Solarus Lua API (nullptr means no callbacks for this sprite). TODO move this to ExportableToLua */

    // animation set
    static std::map<std::string, SpriteAnimationSet*> all_animation_sets;
    const std::string animation_set_id;  /**< id of this sprite's animation set */
    SpriteAnimationSet& animation_set;   /**< animation set of this sprite */

    // current state of the sprite

    std::string current_animation_name;  /**< name of the current animation */
    SpriteAnimation* current_animation;  /**< the current animation */
    int current_direction;             /**< current direction of the animation (the first one is number 0);
                                        * it can be different from the movement direction
                                        * of the entity, because sometimes a sprite can
                                        * go backwards. */
    int current_frame;                 /**< current frame of the animation (the first one is number 0) */
    bool frame_changed;                /**< indicates that the frame has just changed */

    uint32_t frame_delay;              /**< delay between two frames in milliseconds */
    uint32_t next_frame_date;          /**< date of the next frame */

    bool ignore_suspend;               /**< true to continue playing the animation even when the game is suspended */
    bool paused;                       /**< true if the animation is paused */
    bool finished;                     /**< true if the animation has been stopped because the last frame is finished */
    SpritePtr
        synchronize_to;                /**< another sprite to synchronize the frame to
                                        * when they have the same animation name (or nullptr) */

    // effects
    mutable SurfacePtr
        intermediate_surface;          /**< an intermediate surface used to show transitions and other effects */
    uint32_t blink_delay;              /**< blink delay of the sprite, or zero if the sprite is not blinking */
    bool blink_is_sprite_visible;      /**< when blinking, true if the sprite is visible or false if it is invisible */
    uint32_t blink_next_change_date;   /**< date of the next change when blinking: visible or not */

};

}

#endif

