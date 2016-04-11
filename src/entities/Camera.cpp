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
#include "solarus/entities/Camera.h"
#include "solarus/entities/Entities.h"
#include "solarus/entities/EntityState.h"
#include "solarus/entities/Hero.h"
#include "solarus/entities/Separator.h"
#include "solarus/lowlevel/System.h"
#include "solarus/lowlevel/Video.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/movements/TargetMovement.h"
#include "solarus/Game.h"
#include "solarus/Map.h"

#include <algorithm>
#include <list>

namespace Solarus {

namespace {

/**
 * \brief State of the camera when centered on an entity.
 */
class TrackingState: public Entity::State {

public:

  TrackingState(Camera& camera, const EntityPtr& tracked_entity);

  void update() override;
  bool is_traversing_separator() const;
  void traverse_separator(Separator& separator);

  const EntityPtr& get_tracked_entity() const;

private:

  EntityPtr tracked_entity;               /**< Entity the camera is tracking. */
  Rectangle separator_scrolling_position; /**< Current camera position while crossing a separator. */
  Rectangle separator_target_position;    /**< Target camera position when crossing a separator. */
  Point separator_scrolling_delta;        /**< increment to the camera position when crossing a separator. */
  uint32_t separator_next_scrolling_date; /**< Next camera position change when crossing a separator. */
  int separator_scrolling_direction4;     /**< Direction when scrolling. */
  std::shared_ptr<Separator>
      separator_traversed;                /**< Separator currently being traversed or nullptr. */

};

/**
 * \brief Creates a camera tracking state.
 * \param camera The camera to control.
 * \param tracked_entity The entity to track with this camera.
 */
TrackingState::TrackingState(Camera& camera, const EntityPtr& tracked_entity) :
  Entity::State(camera, "tracking"),
  tracked_entity(tracked_entity),
  separator_next_scrolling_date(0),
  separator_scrolling_direction4(0) {

  Debug::check_assertion(tracked_entity != nullptr,
      "Missing tracked entity");
}

/**
 * \brief Returns the entity tracked in this state.
 * \return The tracked entity.
 */
const EntityPtr& TrackingState::get_tracked_entity() const {
  return tracked_entity;
}

/**
 * \brief Updates the position of the camera when the camera is tracking
 * an entity.
 */
void TrackingState::update() {

  Camera& camera = get_entity<Camera>();
  if (separator_next_scrolling_date == 0) {
    // Normal case: not traversing a separator.

    // First compute camera coordinates ignoring map limits and separators.
    Rectangle next = camera.get_bounding_box();
    next.set_center(tracked_entity->get_center_point());

    // Then apply constraints of both separators and map limits.
    camera.set_bounding_box(camera.apply_separators_and_map_bounds(next));
    camera.notify_bounding_box_changed();
  }
  else {
    // The tracked entity is currently traversing a separator.
    // Update camera coordinates.
    uint32_t now = System::now();
    bool finished = false;
    while (separator_next_scrolling_date != 0
        && now >= separator_next_scrolling_date) {
      separator_scrolling_position.add_xy(separator_scrolling_delta);

      separator_next_scrolling_date += 1;

      if (separator_scrolling_position == separator_target_position) {
        // Finished.
        finished = true;
      }
    }

    if (finished) {
      separator_next_scrolling_date = 0;
      separator_traversed->notify_activated(separator_scrolling_direction4);
      separator_traversed = nullptr;
      separator_scrolling_direction4 = 0;
    }

    // Then only apply map limit constraints.
    // Ignore separators since we are currently crossing one of them.
    camera.set_bounding_box(camera.apply_map_bounds(separator_scrolling_position));
    camera.notify_bounding_box_changed();
  }
}

/**
 * \brief Returns whether the camera is currently scrolling on a separator.
 * \return \c truc if scrolling on a separator.
 */
bool TrackingState::is_traversing_separator() const {
  return separator_traversed != nullptr;
}

/**
 * \brief Starts traversing a separator.
 *
 * The tracked entity must touch the separator when you call this function.
 *
 * \param separator The separator to traverse.
 */
void TrackingState::traverse_separator(Separator& separator) {

  Camera& camera = get_entity<Camera>();

  // Save the current position of the camera.
  separator_scrolling_position = camera.get_bounding_box();

  // Start scrolling.
  separator_traversed = std::static_pointer_cast<Separator>(
      separator.shared_from_this()
  );
  separator_scrolling_delta = Point();
  separator_target_position = separator_scrolling_position;

  const Point& tracked_entity_center = tracked_entity->get_center_point();
  const Point& separator_center = separator.get_center_point();
  if (separator.is_horizontal()) {
    if (tracked_entity_center.y < separator_center.y) {
      separator_scrolling_direction4 = 3;
      separator_scrolling_delta.y = 1;
      separator_target_position.add_y(camera.get_height());
    }
    else {
      separator_scrolling_direction4 = 1;
      separator_scrolling_delta.y = -1;
      separator_target_position.add_y(-camera.get_height());
    }
  }
  else {
    if (tracked_entity_center.x < separator_center.x) {
      separator_scrolling_direction4 = 0;
      separator_scrolling_delta.x = 1;
      separator_target_position.add_x(camera.get_width());
    }
    else {
      separator_scrolling_direction4 = 2;
      separator_scrolling_delta.x = -1;
      separator_target_position.add_x(-camera.get_width());
    }
  }

  separator.notify_activating(separator_scrolling_direction4);
  separator_next_scrolling_date = System::now();

  // Move the tracked entity two pixels ahead to avoid to traverse the separator again.
  tracked_entity->set_xy(tracked_entity->get_xy() + 2 * separator_scrolling_delta);
  tracked_entity->notify_bounding_box_changed();
}

/**
 * \brief State of the camera when controlled by scripts.
 */
class ManualState: public Entity::State {

public:

  ManualState(Camera& camera);

};

/**
 * \brief Creates a camera manual state.
 * \param camera The camera to control.
 */
ManualState::ManualState(Camera& camera) :
  Entity::State(camera, "manual") {

}

}  // Anonymous namespace.

/**
 * \brief Creates a camera.
 * \param map The map.
 */
Camera::Camera(Map& map):
  Entity("", 0, map.get_max_layer(), Point(0, 0), Video::get_quest_size()),
  surface(nullptr),
  position_on_screen(0, 0) {

  create_surface();
  set_map(map);
  const HeroPtr& hero = get_game().get_hero();
  Debug::check_assertion(hero != nullptr, "Missing hero when initializing camera");
  start_tracking(hero);
}

/**
 * \copydoc Entity::get_type()
 */
EntityType Camera::get_type() const {
  return ThisType;
}

/**
 * \copydoc Entity::can_be_drawn()
 */
bool Camera::can_be_drawn() const {

  // The camera itself is not drawn.
  // Entities only use its position to draw the map.
  return false;
}

/**
 * \brief Initializes the surface where this camera draws entities.
 *
 * This function should be called when the camera size is changed.
 */
void Camera::create_surface() {

  surface = Surface::create(get_size());
  surface->set_software_destination(false);
}

/**
 * \brief Returns the surface where this camera draws entities.
 * \return The camera surface.
 */
const SurfacePtr& Camera::get_surface() {
  return surface;
}

/**
 * \brief Notifies this entity that its size has just changed.
 */
void Camera::notify_size_changed() {

  // The size thas changed: rebuild the surface.
  if (surface == nullptr || get_size() != surface->get_size()) {
    create_surface();
  }
}

/**
 * \brief Returns where this camera is displayed on the screen.
 * \return Position of the upper-left corner of the camera relative to the
 * quest screen.
 */
Point Camera::get_position_on_screen() const {
  return position_on_screen;
}

/**
 * \brief Sets where this camera is displayed on the screen.
 * \param position_on_screen position of the upper-left corner of the camera
 * relative to the quest screen.
 */
void Camera::set_position_on_screen(const Point& position_on_screen) {
  this->position_on_screen = position_on_screen;
}

/**
 * \brief Returns the position this camera should take to track the specified point.
 * \param tracked_xy A point in map coordinates.
 * \return Where this camera should be placed to be focused on this point,
 * respecting separators and map limits.
 */
Point Camera::get_position_to_track(const Point& tracked_xy) const {

  Point top_left_xy = tracked_xy - Point(get_width() / 2, get_height() / 2);
  Rectangle box(top_left_xy, get_size());

  return apply_separators_and_map_bounds(box).get_xy();
}

/**
 * \copydoc Entity::set_suspended()
 *
 * Reimplemented from Entity to do nothing, in particular to avoid suspending
 * the camera movement.
 *
 * TODO rename set_suspended() to notify_suspended() / notify_unsuspended().
 */
void Camera::set_suspended(bool /* suspended */) {

}

/**
 * \copydoc Entity::notify_movement_started()
 */
void Camera::notify_movement_started() {

  Entity::notify_movement_started();

  // When a movement is set, automatically switch to manual state.
  if (get_state_name() != "manual") {
    start_manual();
  }
}

/**
 * \brief Makes that the camera track the given entity.
 * \param tracked_entity The entity to track.
 */
void Camera::start_tracking(const EntityPtr& tracked_entity) {
  set_state(new TrackingState(*this, tracked_entity));
}

/**
 * \brief Makes the camera stop tracking any entity.
 */
void Camera::start_manual() {
  set_state(new ManualState(*this));
}

/**
 * \brief Returns the entity currently tracked by this camera if any.
 * \return The tracked entity, or nullptr if the camera is not in tracking
 * state.
 */
EntityPtr Camera::get_tracked_entity() const {

  if (get_state_name() != "tracking") {
    return nullptr;
  }

  return static_cast<TrackingState&>(get_state()).get_tracked_entity();
}

/**
 * \brief Function called when the tracked entity crosses a separator.
 * \param separator The separator being traversed.
 */
void Camera::notify_tracked_entity_traversing_separator(Separator& separator) {

  if (get_state_name() != "tracking") {
    return;
  }

  static_cast<TrackingState&>(get_state()).traverse_separator(separator);
}

/**
 * \brief Returns whether this camera is currently scrolling on a separator.
 * \return \c true if this camera is tracking an entity that traverses a
 * separator.
 */
bool Camera::is_traversing_separator() const {

  if (get_state_name() != "tracking") {
    return false;
  }

  return static_cast<TrackingState&>(get_state()).is_traversing_separator();
}

/**
 * \brief Ensures that a rectangle does not cross map limits.
 * \param area The rectangle to check. Is should not be entirely inside the map.
 * It can be bigger than the map: in such a case, the resulting rectangle cannot
 * avoid to cross map limits, and it will be centered.
 * \return A rectangle corresponding to the first one but stopping on map limits.
 */
Rectangle Camera::apply_map_bounds(const Rectangle& area) const {

  int x = area.get_x();  // Top-left corner.
  int y = area.get_y();
  const int width = area.get_width();
  const int height = area.get_height();

  const Size& map_size = get_map().get_size();
  if (map_size.width < width) {
    x = (map_size.width - width) / 2;
  }
  else {
    x = std::min(std::max(x, 0),
        map_size.width - width);
  }

  if (map_size.height < height) {
    y = (map_size.height - height) / 2;
  }
  else {
    y = std::min(std::max(y, 0),
        map_size.height - height);
  }
  return Rectangle(x, y, width, height);
}

/**
 * \brief Ensures that a rectangle does not cross separators.
 * \param area The rectangle to check.
 * \return A rectangle corresponding to the first one but stopping on separators.
 */
Rectangle Camera::apply_separators(const Rectangle& area) const {

  int x = area.get_x();  // Top-left corner.
  int y = area.get_y();
  const int width = area.get_width();
  const int height = area.get_height();

  // TODO simplify: treat horizontal separators first and then all vertical ones.
  int adjusted_x = x;  // Updated coordinates after applying separators.
  int adjusted_y = y;
  std::vector<std::shared_ptr<const Separator>> applied_separators;
  const std::set<std::shared_ptr<const Separator>>& separators =
      get_entities().get_entities_by_type<Separator>();
  for (const std::shared_ptr<const Separator>& separator: separators) {

    if (separator->is_vertical()) {
      // Vertical separator.
      int separation_x = separator->get_x() + 8;

      if (x < separation_x && separation_x < x + width
          && separator->get_y() < y + height
          && y < separator->get_y() + separator->get_height()) {
        int left = separation_x - x;
        int right = x + width - separation_x;
        if (left > right) {
          adjusted_x = separation_x - width;
        }
        else {
          adjusted_x = separation_x;
        }
        applied_separators.push_back(separator);
      }
    }
    else {
      Debug::check_assertion(separator->is_horizontal(), "Invalid separator shape");

      // Horizontal separator.
      int separation_y = separator->get_y() + 8;
      if (y < separation_y && separation_y < y + height
          && separator->get_x() < x + width
          && x < separator->get_x() + separator->get_width()) {
        int top = separation_y - y;
        int bottom = y + height - separation_y;
        if (top > bottom) {
          adjusted_y = separation_y - height;
        }
        else {
          adjusted_y = separation_y;
        }
        applied_separators.push_back(separator);
      }
    }
  }  // End for each separator.

  bool must_adjust_x = true;
  bool must_adjust_y = true;
  if (adjusted_x != x && adjusted_y != y) {
    // Both directions were modified. Maybe it is a T configuration where
    // a separator deactivates another one.

    must_adjust_x = false;
    must_adjust_y = false;
    for (const std::shared_ptr<const Separator>& separator: applied_separators) {

      if (separator->is_vertical()) {
        // Vertical separator.
        int separation_x = separator->get_x() + 8;

        if (x < separation_x && separation_x < x + width
            && separator->get_y() < adjusted_y + height
            && adjusted_y < separator->get_y() + separator->get_height()) {
          must_adjust_x = true;
        }
      }
      else {
        // Horizontal separator.
        int separation_y = separator->get_y() + 8;

        if (y < separation_y && separation_y < y + height
            && separator->get_x() < adjusted_x + width
            && adjusted_x < separator->get_x() + separator->get_width()) {
          must_adjust_y = true;
        }
      }
    }  // End for each separator applied.
  }  // End if both directions.

  if (must_adjust_x) {
    x = adjusted_x;
  }
  if (must_adjust_y) {
    y = adjusted_y;
  }

  return Rectangle(x, y, width, height);
}

/**
 * \brief Ensures that a rectangle does not cross separators nor map bounds.
 * \param area The rectangle to check.
 * It is the responsibility of quest makers to put enough space between
 * separators (the space should be at least the quest size).
 * If separators are too close to each other for the rectangle to fit,
 * the camera will cross some of them.
 * If separators are too close to a limit of the map, the
 * the camera will cross them but will never cross map limits.
 * \return A rectangle corresponding to the first one but stopping on
 * separators and map bounds.
 */
Rectangle Camera::apply_separators_and_map_bounds(const Rectangle& area) const {
  return apply_map_bounds(apply_separators(area));
}

}
