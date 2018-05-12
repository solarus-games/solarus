/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/core/Debug.h"
#include "solarus/graphics/Drawable.h"
#include "solarus/graphics/Transition.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/movements/Movement.h"
#include "solarus/graphics/Surface.h"
#include <lua.hpp>
#include <utility>

namespace Solarus {

/**
 * \brief Constructor.
 */
Drawable::Drawable():
  xy(),
  movement(nullptr),
  transition(nullptr),
  transition_callback_ref(),
  suspended(false),
  blend_mode(BlendMode::BLEND),
  opacity(255)
{

}

/**
 * \brief Destructor.
 */
Drawable::~Drawable() {

  stop_transition();
  stop_movement();
}

/**
 * \brief Returns the origin point of the drawable.
 * \return The origin point of the drawable.
 */
Point Drawable::get_origin() const {
  return Point(0, 0);
}

/**
 * \brief Applies a movement to this object.
 *
 * Any previous movement is stopped.
 *
 * \param movement The movement to apply.
 */
void Drawable::start_movement(const std::shared_ptr<Movement>& movement) {

  stop_movement();
  this->movement = movement;
  movement->set_drawable(this);

  movement->set_suspended(is_suspended());
}

/**
 * \brief Stops the movement applied to the object, if any.
 *
 * The movement is deleted unless the owner script uses it elsewhere.
 */
void Drawable::stop_movement() {

  movement = nullptr;
}

/**
 * \brief Returns the current movement of this drawable object.
 * \return The object's movement, or nullptr if there is no movement.
 */
const std::shared_ptr<Movement>& Drawable::get_movement() {
  return movement;
}

/**
 * \brief Returns the coordinates of this drawable object as defined by its
 * movement.
 * \return The coordinates of this drawable object.
 */
const Point& Drawable::get_xy() const {
  return xy;
}

/**
 * \brief Sets the coordinates of this drawable object.
 * \param xy The new coordinates of this drawable object.
 */
void Drawable::set_xy(const Point& xy) {
  this->xy = xy;
}

/**
 * \brief Starts a transition effect on this object.
 *
 * Any previous transition is stopped.
 *
 * \param transition The transition to start.
 * \param callback_ref A Lua registry ref to the function to call when
 * the transition finishes, or an empty ref.
 */
void Drawable::start_transition(
    std::unique_ptr<Transition> transition,
    const ScopedLuaRef& callback_ref
) {
  stop_transition();

  this->transition = std::move(transition);
  this->transition_callback_ref = callback_ref;
  this->transition->start();
  this->transition->set_suspended(is_suspended());
}

/**
 * \brief Stops the transition effect applied to this object, if any.
 *
 * The transition is deleted and the Lua callback (if any) is canceled.
 */
void Drawable::stop_transition() {
  if(transition) transition->finish(*this);
  transition = nullptr;
  transition_callback_ref.clear();
}

/**
 * \brief Returns the current transition of this drawable object.
 * \return The object's transition, or nullptr if there is no transition.
 */
Transition* Drawable::get_transition() {
  return transition.get();
}

/**
 * \brief Updates this object.
 *
 * This function is called repeatedly.
 * You can redefine it for your needs.
 */
void Drawable::update() {

  if (transition != nullptr) {
    transition->update();
    if (transition->is_finished()) {
      transition->finish(*this);
      transition = nullptr;

      if (!transition_callback_ref.is_empty()) {
        // Note that this callback may create a new transition right now.
        transition_callback_ref.clear_and_call("transition callback");
      }
    }
  }

  if (movement != nullptr) {
    movement->update();
    if (movement != nullptr && movement->is_finished()) {
      stop_movement();
    }
  }
}

/**
 * \brief Returns whether this drawable is suspended.
 * \return \c true if this drawable is suspended.
 */
bool Drawable::is_suspended() const {
  return suspended;
}

/**
 * \brief Suspends or resumes this drawable.
 * \param suspended \c true to suspend this drawable, \c false to resume it.
 */
void Drawable::set_suspended(bool suspended) {

  if (suspended == this->suspended) {
    return;
  }

  this->suspended = suspended;

  // Suspend or resume the transition effect and the movement if any.
  if (transition != nullptr) {
    transition->set_suspended(suspended);
  }

  if (movement != nullptr) {
    movement->set_suspended(suspended);
  }
}

/**
 * \brief Draws this object, applying dynamic effects.
 * \param dst_surface the destination surface
 */
void Drawable::draw(const SurfacePtr& dst_surface) {

  draw(dst_surface, Point(0, 0));
}

/**
 * \brief Draws this object, applying dynamic effects.
 * \param dst_surface the destination surface
 * \param x x coordinate of where to draw
 * \param y y coordinate of where to draw
 */
void Drawable::draw(const SurfacePtr& dst_surface, int x, int y) {
  draw(dst_surface, Point(x, y));
}

/**
 * \brief Draws this object, applying dynamic effects.
 * \param dst_surface the destination surface
 * \param dst_position position on this surface
 * (will be added to the position obtained by previous movements)
 */
void Drawable::draw(const SurfacePtr& dst_surface,
    const Point& dst_position) {
  draw_region(get_region(),dst_surface,dst_position);
}

/**
 * \brief Draws a subrectangle of this object, applying dynamic effects.
 * \param region The rectangle to draw in this object.
 * \param dst_surface The destination surface.
 */
void Drawable::draw_region(
    const Rectangle& region,
    const SurfacePtr& dst_surface) {
  draw_region(region, dst_surface, Point(0, 0));
}

/**
 * \brief Draws a subrectangle of this object, applying dynamic effects.
 * \param region The rectangle to draw in this object.
 * \param dst_surface The destination surface
 * \param dst_position Position on this surface
 * (will be added to the position obtained by previous movements).
 */
void Drawable::draw_region(const Rectangle& region,
    const SurfacePtr& dst_surface,
    const Point& dst_position) {
  if (transition) {
    draw_region(region,dst_surface,dst_position,DrawProxyChain<2>({*transition,terminal()}));
  } else {
    draw_region(region, dst_surface, dst_position,terminal());
  }
}

/**
 * \brief Draws a subrectangle of this object, applying dynamic effects.
 * \param region The rectangle to draw in this object.
 * \param dst_surface The destination surface
 * \param dst_position Position on this surface
 * (will be added to the position obtained by previous movements).
 */
void Drawable::draw_region(const Rectangle& region,
    const SurfacePtr& dst_surface,
    const Point& dst_position, const DrawProxy &proxy) {
  Point off_dst = dst_position + xy;
  raw_draw(*dst_surface, DrawInfos(region,off_dst,get_blend_mode(),get_opacity(),proxy));
}

/**
 * @brief set the shader used to draw this drawable
 * @param shader, the shader, must be a valid shader or nullptr
 */
void Drawable::set_shader(const ShaderPtr& shader) {
  this->shader = shader;
}

/**
 * @brief get current shader bound to this drawable
 * @return current shader, can be nullptr if none
 */
const ShaderPtr& Drawable::get_shader() const {
  return shader;
}

/**
 * \brief Returns the blend mode of this drawable object.
 * \return The blend mode.
 */
BlendMode Drawable::get_blend_mode() const {
  return blend_mode;
}

/**
 * \brief Sets the blend mode of this drawable object.
 * \param blend_mode The blend mode.
 */
void Drawable::set_blend_mode(BlendMode blend_mode) {
  this->blend_mode = blend_mode;
}

/**
 * @brief Get object opacity
 * @return the opacity
 */
uint8_t Drawable::get_opacity() const {
  return opacity;
}

/**
 * @brief Sets object's opacity
 * @param the opacity
 */
void Drawable::set_opacity(uint8_t opacity) {
  this->opacity = opacity;
}

const DrawProxy& Drawable::terminal() const {
  if(shader)
    return (const DrawProxy&)(*shader);
  else
    return Surface::draw_proxy;
}

}

