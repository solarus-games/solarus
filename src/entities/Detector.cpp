#include "entities/Detector.h"
#include "ResourceManager.h"
#include "Map.h"
#include "KeysEffect.h"
#include "Sprite.h"
#include "SpriteAnimationSet.h"

/**
 * Constructor.
 * @param collision_mode the detector's collision mode(s)
 * @param name a name identifying the detector (or an empty string if you
 * don't need to identify the detector)
 * @param layer layer of the detector
 * @param x x position of the detector's rectangle
 * @param y y position of the detector's rectangle
 * @param width width of the detector's rectangle
 * @param height height of the detector's rectangle
 */
Detector::Detector(int collision_mode,
		   string name, MapEntity::Layer layer,
		   int x, int y, int width, int height):
  MapEntity(name, 0, layer, x, y, width, height),
  collision_mode(collision_mode), layer_ignored(false) {

}

/**
 * Destructor.
 */
Detector::~Detector(void) {

}

/**
 * Sets the collision mode of this detector.
 * The collision mode can be an OR combination of collision modes.
 * @param collision_mode the detector's collision mode
 */
void Detector::set_collision_mode(int collision_mode) {

  if (collision_mode & COLLISION_SPRITE) {
    get_sprite(0)->get_animation_set()->enable_pixel_collisions();
  }

  this->collision_mode = collision_mode;
}

/**
 * Adds a collision mode to this detector.
 * @param collision_mode the collision mode to add
 */
void Detector::add_collision_mode(CollisionMode collision_mode) {

  set_collision_mode(this->collision_mode | collision_mode);
}

/**
 * Sets whether the detector can detect entities even if
 * they are not on the same layer.
 * @param layer_ignored true to detect all entities, false
 * to detect only those on the same layer.
 */
void Detector::set_layer_ignored(bool layer_ignored) {
  this->layer_ignored = layer_ignored;
}

/**
 * This function is called by the map when an entity has just moved.
 * It checks whether the entity collides with this detector.
 * Depending on the detector collision mode(s), the appropriate check_collision_*
 * functions are called.
 * If there is a collision, the collision(MapEntity*) method is called.
 * @param entity the entity to check
 */
void Detector::check_collision(MapEntity *entity) {
  
  if (entity != this
      && (layer_ignored || get_layer() == entity->get_layer())) { // the entity is in the same layer as the detector
  
    // detect the collision depending on the collision mode
    
    if (collision_mode & COLLISION_RECTANGLE) {
      check_collision_rectangle(entity);
    }

    if (collision_mode & COLLISION_ORIGIN_POINT) {
      check_collision_origin_point(entity);
    }

    if (collision_mode & COLLISION_FACING_POINT) {
      check_collision_facing_point(entity);
    }
  }
}

/**
 * This function whether a sprite collides with this detector.
 * Depending on the detector collision mode(s), the appropriate check_collision_*
 * functions are called.
 * If there is a collision, the collision(MapEntity*, Sprite*) method is called.
 * @param sprite the sprite to check
 */
void Detector::check_collision(MapEntity *entity, Sprite *sprite) {

  if ((collision_mode & COLLISION_SPRITE)
      && entity != this
      && (layer_ignored || get_layer() == entity->get_layer())
      && sprite->get_animation_set()->are_pixel_collisions_enabled()
      && get_sprite(0)->check_collision(sprite, get_x(), get_y(), entity->get_x(), entity->get_y())) {

    collision(entity, sprite);
  }
}

/**
 * Checks whether an entity's rectangle is overlapping the detector's rectangle.
 * This method is called by check_collision(MapEntity*) when the detector's collision
 * mode is COLLISION_RECTANGLE.
 * @param entity the entity
 * @return true if the entity's rectangle is overlapping the detector's rectangle
 */
void Detector::check_collision_rectangle(MapEntity *entity) {

  if (entity->overlaps(get_position_in_map())) {

    // notify the detector
    collision(entity, COLLISION_RECTANGLE);
  }
}

/**
 * Checks whether an entity's origin point is overlapping the detector's rectangle.
 * This method is called by check_entity_collision(MapEntity*) when the detector's collision
 * mode is COLLISION_ORIGIN_POINT.
 * @param entity the entity
 * @return true if the entity's origin point is overlapping the detector's rectangle
 */
void Detector::check_collision_origin_point(MapEntity *entity) {

  if (entity->is_origin_point_in(get_position_in_map())) {

    // notify the detector
    collision(entity, COLLISION_ORIGIN_POINT);
  }
}

/**
 * Checks whether an entity's facing point is overlapping the detector's rectangle.
 * This method is called by check_collision(MapEntity*) when the detector's collision
 * mode is COLLISION_FACING_POINT.
 * @param entity the entity
 * @return true if the entity's facing point is overlapping the detector's rectangle
 */
void Detector::check_collision_facing_point(MapEntity *entity) {

  if (entity->is_facing_point_in(get_position_in_map())) {

    entity->set_facing_entity(this);
    collision(entity, COLLISION_FACING_POINT);
  }
}

/**
 * This function is called by check_collision(MapEntity*)
 * when an entity overlaps the detector.
 * By default, the map is notified.
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision (useful if
 * the detector has several collision modes)
 */
void Detector::collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {

  map->event_entity_on_detector(this, entity_overlapping);
}

/**
 * This function is called by check_collision(MapEntity*, Sprite*)
 * when a sprite overlaps the detector.
 * By default, nothing happens.
 * Redefine this method in the subclasses to do the appropriate action.
 * @param entity the entity
 * @param sprite_overlapping the sprite of this entity overlapping the detector
 */
void Detector::collision(MapEntity *entity, Sprite *sprite_overlapping) {

}

/**
 * This function is called when the player presses the action key
 * when Link is facing this detector, and the action icon lets him do this.
 * By default, nothing happens.
 * Redefine your function in the subclasses to allow Link to interact with this entity.
 */
void Detector::action_key_pressed(void) {

}
