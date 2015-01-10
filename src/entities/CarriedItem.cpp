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
#include "solarus/entities/CarriedItem.h"
#include "solarus/entities/Destructible.h"
#include "solarus/entities/Hero.h"
#include "solarus/entities/Enemy.h"
#include "solarus/entities/Explosion.h"
#include "solarus/entities/Sensor.h"
#include "solarus/entities/Stairs.h"
#include "solarus/entities/Switch.h"
#include "solarus/entities/Crystal.h"
#include "solarus/entities/Npc.h"
#include "solarus/entities/MapEntities.h"
#include "solarus/movements/PixelMovement.h"
#include "solarus/movements/FollowMovement.h"
#include "solarus/movements/StraightMovement.h"
#include "solarus/Sprite.h"
#include "solarus/Game.h"
#include "solarus/Map.h"
#include "solarus/lowlevel/System.h"
#include "solarus/lowlevel/Sound.h"
#include "solarus/lowlevel/Geometry.h"

namespace Solarus {

/**
 * \brief Movement of the item when the hero is lifting it.
 */
const std::string CarriedItem::lifting_trajectories[4] = {
    "0 0  0 0  -3 -3  -5 -3  -5 -2",
    "0 0  0 0  0 -1  0 -1  0 0",
    "0 0  0 0  3 -3  5 -3  5 -2",
    "0 0  0 0  0 -10  0 -12  0 0"
};

/**
 * \brief Creates a carried item (i.e. an item carried by the hero).
 * \param hero the hero who is lifting the item to be created
 * \param original_entity the entity that will be replaced by this carried item
 * (its coordinates, size and origin will be copied)
 * \param animation_set_id name of the animation set for the sprite to create
 * \param destruction_sound_id Name of the sound to play when this item is destroyed
 * (or an empty string).
 * \param damage_on_enemies damage received by an enemy if the item is thrown on him
 * (possibly 0)
 * \param explosion_date date of the explosion if the item should explode,
 * or 0 if the item does not explode
 */
CarriedItem::CarriedItem(
    Hero& hero,
    const MapEntity& original_entity,
    const std::string& animation_set_id,
    const std::string& destruction_sound_id,
    int damage_on_enemies,
    uint32_t explosion_date
):
  MapEntity("", 0, hero.get_layer(), Point(0, 0), Size(0, 0)),
  hero(hero),
  is_lifting(true),
  is_throwing(false),
  is_breaking(false),
  break_one_layer_above(false),
  destruction_sound_id(destruction_sound_id),
  damage_on_enemies(damage_on_enemies),
  shadow_sprite(nullptr),
  throwing_direction(0),
  next_down_date(0),
  item_height(0),
  y_increment(0),
  explosion_date(explosion_date) {

  // align correctly the item with the hero
  int direction = hero.get_animation_direction();
  if (direction % 2 == 0) {
    set_xy(original_entity.get_x(), hero.get_y());
  }
  else {
    set_xy(hero.get_x(), original_entity.get_y());
  }
  set_origin(original_entity.get_origin());
  set_size(original_entity.get_size());
  set_drawn_in_y_order(true);

  // create the lift movement and the sprite
  std::shared_ptr<PixelMovement> movement = std::make_shared<PixelMovement>(
      lifting_trajectories[direction], 100, false, true
  );
  create_sprite(animation_set_id);
  get_sprite().set_current_animation("stopped");
  set_movement(movement);

  // create the shadow (not visible yet)
  shadow_sprite = std::make_shared<Sprite>("entities/shadow");
  shadow_sprite->set_current_animation("big");
}

/**
 * \brief Returns the type of entity.
 * \return the type of entity
 */
EntityType CarriedItem::get_type() const {
  return EntityType::CARRIED_ITEM;
}

/**
 * \brief Returns whether entities of this type can be obstacles for other entities.
 * \return true if this type of entity can be obstacle for other entities
 */
bool CarriedItem::can_be_obstacle() const {
  return false;
}

/**
 * \brief Returns whether this entity is sensible to the ground below it.
 * \return \c true if this entity is sensible to its ground.
 */
bool CarriedItem::is_ground_observer() const {
  return true;  // To make the item fall into holes, water, etc.
}

/**
 * \brief Returns the damage this item can cause to ennemies.
 * \return the damage on enemies
 */
int CarriedItem::get_damage_on_enemies() const {
  return damage_on_enemies;
}

/**
 * \brief Makes the item sprite stop moving.
 *
 * This function is called when the hero stops walking while carrying the item.
 * The item also stops moving.
 */
void CarriedItem::set_animation_stopped() {

  if (!is_lifting && !is_throwing) {
    std::string animation = will_explode_soon() ? "stopped_explosion_soon" : "stopped";
    get_sprite().set_current_animation(animation);
  }
}

/**
 * \brief Makes the item sprite move.
 *
 * This function is called when the hero starts walking while carrying the item.
 * The item moves like him.
 */
void CarriedItem::set_animation_walking() {

  if (!is_lifting && !is_throwing) {
    std::string animation = will_explode_soon() ? "walking_explosion_soon" : "walking";
    get_sprite().set_current_animation(animation);
  }
}

/**
 * \brief Throws the item.
 * \param direction direction where the hero throws the item (0 to 3)
 */
void CarriedItem::throw_item(int direction) {

  this->throwing_direction = direction;
  this->is_lifting = false;
  this->is_throwing = true;

  // play the sound
  Sound::play("throw");

  // stop the sprite animation
  Sprite& sprite = get_sprite();
  sprite.set_current_animation("stopped");

  // set the movement of the item sprite
  set_y(hero.get_y());
  std::shared_ptr<StraightMovement> movement =
      std::make_shared<StraightMovement>(false, false);
  movement->set_speed(200);
  movement->set_angle(Geometry::degrees_to_radians(direction * 90));
  clear_movement();
  set_movement(movement);

  this->y_increment = -2;
  this->next_down_date = System::now() + 40;
  this->item_height = 18;
}

/**
 * \brief Returns whether the item is being lifted.
 * \return true if the item is being lifted
 */
bool CarriedItem::is_being_lifted() const {
  return is_lifting;
}

/**
 * \brief Returns whether the item is being thrown.
 * \return true if the item is being thrown
 */
bool CarriedItem::is_being_thrown() const {
  return is_throwing;
}

/**
 * \brief Returns whether the item is about to explode.
 * \return true if the item is about to explode
 */
bool CarriedItem::will_explode_soon()  const{
  return can_explode() && System::now() >= explosion_date - 1500;
}

/**
 * \brief Destroys the item while it is being thrown.
 */
void CarriedItem::break_item() {

  if (is_throwing && throwing_direction != 3) {
    // destroy the item where it is actually drawn
    set_y(get_y() - item_height);
  }

  get_movement()->stop();

  if (!can_explode()) {
    if (!destruction_sound_id.empty()) {
      Sound::play(destruction_sound_id);
    }
    if (get_sprite().has_animation("destroy")) {
      get_sprite().set_current_animation("destroy");
    }
  }
  else {
    get_entities().add_entity(std::make_shared<Explosion>(
        "", get_layer(), get_xy(), true
    ));
    Sound::play("explosion");
    if (is_throwing) {
      remove_from_map(); // because if the item was still carried by the hero, then the hero class will destroy it
    }
  }
  is_throwing = false;
  is_breaking = true;
}

/**
 * \brief Destroys the item after it finishes its thrown movement.
 *
 * How the item breaks depends on the ground where it lands.
 */
void CarriedItem::break_item_on_ground() {

  get_movement()->stop();

  Ground ground = get_ground_below();
  switch (ground) {

    case Ground::EMPTY:
      // Nothing here: fall one layer below.
    {
      int layer = get_layer();
      if (layer == LAYER_LOW) {
        // Cannot fall lower.
        break_item();
      }
      else {
        get_entities().set_entity_layer(*this, Layer(layer - 1));
        break_item_on_ground();  // Do this again on the next layer.
      }
      break;
    }

    case Ground::HOLE:
      Sound::play("jump");
      remove_from_map();
      break;

    case Ground::DEEP_WATER:
    case Ground::LAVA:
      Sound::play("walk_on_water");
      remove_from_map();
      break;

    default:
      // Break the item normally.
      break_item();
      break;
  }

  is_throwing = false;
  is_breaking = true;
}

/**
 * \brief Returns whether the item is broken.
 * \return true if the item is broken
 */
bool CarriedItem::is_broken() const {
  return is_breaking && (get_sprite().is_animation_finished() || can_explode());
}

/**
 * \brief Returns whether the item can explode.
 * \return true if the item will explode
 */
bool CarriedItem::can_explode() const {
  return explosion_date != 0;
}

/**
 * \brief This function is called by the map when the game is suspended or resumed.
 * \param suspended true to suspend the entity, false to resume it
 */
void CarriedItem::set_suspended(bool suspended) {

  MapEntity::set_suspended(suspended); // suspend the animation and the movement

  if (is_throwing) {
    // suspend the shadow
    shadow_sprite->set_suspended(suspended);
  }

  if (!suspended && get_when_suspended() != 0) {
    // recalculate the timers
    uint32_t diff = System::now() - get_when_suspended();
    if (is_throwing) {
      next_down_date += diff;
    }
    if (can_explode()) {
      explosion_date += diff;
    }
  }
}

/**
 * \brief This function is called repeatedly.
 */
void CarriedItem::update() {

  // update the sprite and the position
  MapEntity::update();

  if (is_suspended()) {
    return;
  }

  // when the hero finishes lifting the item, start carrying it
  if (is_lifting && get_movement()->is_finished()) {
    is_lifting = false;

    // make the item follow the hero
    clear_movement();
    set_movement(std::make_shared<FollowMovement>(
        std::static_pointer_cast<Hero>(hero.shared_from_this()),
        0,
        -18,
        true
    ));
  }

  // when the item has finished flying, destroy it
  else if (can_explode() && !is_breaking) {

    uint32_t now = System::now();

    if (now >= explosion_date) {
      break_item();
    }
    else if (will_explode_soon()) {

      std::string animation = get_sprite().get_current_animation();
      if (animation == "stopped") {
        get_sprite().set_current_animation("stopped_explosion_soon");
      }
      else if (animation == "walking") {
        get_sprite().set_current_animation("walking_explosion_soon");
      }
    }
  }

  if (is_throwing) {
    shadow_sprite->update();

    if (is_broken()) {
      remove_from_map();
    }
    else if (break_one_layer_above) {
      break_item();
      Layer layer = get_layer();
      if (layer != LAYER_HIGH) {
        get_entities().set_entity_layer(*this, Layer(layer + 1));
      }
      break_one_layer_above = false;
    }
    else if (get_movement()->is_stopped() || y_increment >= 7) {
      // Interrupt the movement.
      break_item_on_ground();
    }
    else {
      uint32_t now = System::now();
      while (now >= next_down_date) {
        next_down_date += 40;
        item_height -= y_increment;
        y_increment++;
      }
    }
  }
}

/**
 * \brief Notifies this entity that it has just failed to change its position
 * because of obstacles.
 */
void CarriedItem::notify_obstacle_reached() {

  if (is_throwing && !is_broken()) {
    break_item();
  }
}

/**
 * \brief Draws the carried item on the map.
 *
 * This is a redefinition of MapEntity::draw_on_map()
 * to draw the shadow independently of the item movement.
 */
void CarriedItem::draw_on_map() {

  if (!is_drawn()) {
    return;
  }

  if (!is_throwing) {
    // draw the sprite normally
    MapEntity::draw_on_map();
  }
  else {
    // when the item is being thrown, draw the shadow and the item separately
    // TODO: this could probably be simplified by using a JumpMovement
    get_map().draw_sprite(*shadow_sprite, get_xy());
    get_map().draw_sprite(get_sprite(), get_x(), get_y() - item_height);
  }
}

/**
 * \brief This function is called when this carried item collides an enemy.
 * \param enemy the enemy
 */
void CarriedItem::notify_collision_with_enemy(Enemy &enemy) {

  if (is_throwing
      && !can_explode()
      && get_damage_on_enemies() > 0) {
    enemy.try_hurt(EnemyAttack::THROWN_ITEM, *this, nullptr);
  }
}

/**
 * \copydoc MapEntity::notify_attacked_enemy
 */
void CarriedItem::notify_attacked_enemy(
    EnemyAttack /* attack */,
    Enemy& /* victim */,
    const Sprite* /* victim_sprite */,
    EnemyReaction::Reaction& result,
    bool /* killed */) {

  if (result.type != EnemyReaction::ReactionType::IGNORED) {
    break_item();
  }
}

/**
 * \brief Returns whether a teletransporter is currently considered as an obstacle for this entity.
 * \param teletransporter a teletransporter
 * \return true if the teletransporter is currently an obstacle for this entity
 */
bool CarriedItem::is_teletransporter_obstacle(Teletransporter& /* teletransporter */) {
  return false;
}

/**
 * \copydoc MapEntity::is_stream_obstacle
 */
bool CarriedItem::is_stream_obstacle(Stream& /* stream */) {
  return false;
}

/**
 * \brief Returns whether some stairs are currently considered as an obstacle for this entity.
 * \param stairs an stairs entity
 * \return true if the stairs are currently an obstacle for this entity
 */
bool CarriedItem::is_stairs_obstacle(Stairs& /* stairs */) {
  return false;
}

/**
 * \brief Returns whether a low wall is currently considered as an obstacle
 * by this entity.
 * \return \c true if low walls are currently obstacle for this entity.
 */
bool CarriedItem::is_low_wall_obstacle() const {
  return false;
}

/**
 * \brief Returns whether a deep water tile is currently considered as an obstacle for this entity.
 * \return true if the deep water tiles are currently an obstacle for this entity
 */
bool CarriedItem::is_deep_water_obstacle() const {
  return false;
}

/**
 * \brief Returns whether a hole is currently considered as an obstacle for this entity.
 * \return true if the holes are currently an obstacle for this entity
 */
bool CarriedItem::is_hole_obstacle() const {
  return false;
}

/**
 * \brief Returns whether lava is currently considered as an obstacle for this entity.
 * \return true if lava is currently an obstacle for this entity
 */
bool CarriedItem::is_lava_obstacle() const {
  return false;
}

/**
 * \brief Returns whether prickles are currently considered as an obstacle for this entity.
 * \return true if prickles are currently an obstacle for this entity
 */
bool CarriedItem::is_prickle_obstacle() const {
  return false;
}

/**
 * \brief Returns whether a ladder is currently considered as an obstacle for this entity.
 * \return true if the ladders are currently an obstacle for this entity
 */
bool CarriedItem::is_ladder_obstacle() const {
  return false;
}

/**
 * \brief Returns whether a switch is currently considered as an obstacle by this entity.
 * \param sw a switch
 * \return true if the switch is currently an obstacle for this entity
 */
bool CarriedItem::is_switch_obstacle(Switch& /* sw */) {
  return !is_being_thrown();
}

/**
 * \brief Returns whether a raised crystal block is currently considered as an obstacle for this entity.
 * \param raised_block a crystal block raised
 * \return false
 */
bool CarriedItem::is_raised_block_obstacle(CrystalBlock& /* raised_block */) {
  // thrown items can traverse crystal blocks
  return false;
}

/**
 * \brief Returns whether a crystal is currently considered as an obstacle for this entity.
 * \param crystal a crystal
 * \return true if the crystal is currently an obstacle for this entity
 */
bool CarriedItem::is_crystal_obstacle(Crystal& /* crystal */) {
  return !is_being_thrown();
}

/**
 * \brief Returns whether a non-playing character is currently considered as an obstacle for this entity.
 * \param npc a non-playing character
 * \return true if the NPC is currently an obstacle for this entity
 */
bool CarriedItem::is_npc_obstacle(Npc& npc) {
  return npc.is_solid();
}

/**
 * \copydoc MapEntity::is_jumper_obstacle
 */
bool CarriedItem::is_jumper_obstacle(Jumper& /* jumper */, const Rectangle& /* candidate_position */) {
  return false;
}

/**
 * \brief Returns whether a sensor is currently considered as an obstacle for this entity.
 * \param sensor a sensor
 * \return true if this sensor is currently an obstacle for this entity.
 */
bool CarriedItem::is_sensor_obstacle(Sensor& /* sensor */) {
  return false;
}

/**
 * \brief Returns whether an enemy character is considered as an obstacle for this entity.
 * \param enemy an enemy
 * \return true if this enemy is considered as an obstacle for this entity.
 */
bool CarriedItem::is_enemy_obstacle(Enemy& /* enemy */) {
  // if this item explodes when reaching an obstacle, then we consider enemies as obstacles
  return can_explode();
}

/**
 * \brief This function is called when a switch detects a collision with this entity.
 * \param sw the switch
 * \param collision_mode the collision mode that detected the event
 */
void CarriedItem::notify_collision_with_switch(Switch& sw, CollisionMode collision_mode) {

  if (collision_mode == COLLISION_OVERLAPPING
      && is_being_thrown()
      && !can_explode()) {

    sw.try_activate();
    break_item();
  }
}

/**
 * \brief This function is called when a crystal detects a collision with this entity.
 * \param crystal the crystal
 * \param collision_mode the collision mode that detected the event
 */
void CarriedItem::notify_collision_with_crystal(Crystal& crystal, CollisionMode collision_mode) {

  if (collision_mode == COLLISION_OVERLAPPING
      && is_being_thrown()
      && !can_explode()) {

    crystal.activate(*this);
    break_item();
  }
}

/**
 * \brief This function is called when some stairs detect a collision with this entity.
 * \param stairs the stairs entity
 * \param collision_mode the collision mode that detected the event
 */
void CarriedItem::notify_collision_with_stairs(Stairs& stairs, CollisionMode /* collision_mode */) {

  if (is_throwing
      && !is_breaking
      && stairs.is_inside_floor()
      && get_layer() == stairs.get_layer()) {
    break_one_layer_above = true; // show the destruction animation above the stairs
  }
}

}

