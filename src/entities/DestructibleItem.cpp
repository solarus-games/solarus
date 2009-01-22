#include "entities/DestructibleItem.h"
#include "entities/Link.h"
#include "entities/MapEntities.h"
#include "movements/PathMovement.h"
#include "ResourceManager.h"
#include "Game.h"
#include "KeysEffect.h"
#include "Equipment.h"
#include "Map.h"
#include "Sound.h"
#include "ZSDX.h"
#include "Sprite.h"

/**
 * Properties of each type of destructible item.
 */
const DestructibleItem::ItemProperties DestructibleItem::properties[] = {
  {"entities/pot", "stone", true, false, 0},
  {"entities/skull", "stone", true, false, 0},
  {"entities/bush", "bush", true, true, 1},
  {"entities/stone_small_white", "stone", true, false, 1},
  {"entities/stone_small_white", "stone", true, false, 2},
  {"entities/grass", "bush", false, true, 0},

  /* not implemented
  {"entities/stone_big_white", "stone", 1},
  {"entities/stone_big_white", "stone", 2},
  */
};

/**
 * Creates a new destructible item with the specified type.
 * @param layer layer of the destructible item to create on the map
 * @param x x coordinate of the destructible item to create
 * @param y y coordinate of the destructible item to create
 * @param type type of destructible item to create
 * @param pickable_item the type of pickable item that appears when the destructible
 * item is lifted or cut
 * @param pickable_item_savegame_variable index of the savegame boolean variable
 * storing the possession state of the pickable item,
 * for certain kinds of pickable items only (a key, a piece of heart...)
 */
DestructibleItem::DestructibleItem(Layer layer, int x, int y, DestructibleItem::ItemType type,
				   PickableItem::ItemType pickable_item, int pickable_item_savegame_variable):
  Detector(COLLISION_NONE, "", layer, x, y, 16, 16),
  type(type), pickable_item(pickable_item),
  pickable_item_savegame_variable(pickable_item_savegame_variable), is_being_cut(false) {

  set_origin(8, 13);
  create_sprite(get_animation_set_id());

  // set the collision mode
  if (properties[type].can_be_lifted) {
    add_collision_mode(COLLISION_FACING_POINT);
  }

  if (properties[type].can_be_cut) {
    add_collision_mode(COLLISION_SPRITE);
  }

  if (type == GRASS) { // display the grass animation under Link
    add_collision_mode(COLLISION_ORIGIN_POINT);
  }
}

/**
 * Destructor.
 */
DestructibleItem::~DestructibleItem(void) {

}

/**
 * Returns the animation set of this destructible item.
 * @return the animations of the sprite
 */
string DestructibleItem::get_animation_set_id(void) {
  return properties[type].animation_set_id;
}

/**
 * Returns the sound to play when this item is destroyed.
 * @return the sound to play when this item is destroyed
 */
Sound * DestructibleItem::get_destruction_sound(void) {
  return ResourceManager::get_sound(properties[type].destruction_sound_id);
}

/**
 * Returns whether this kind of destructible item is an obstacle.
 * @param true if this is an obstacle
 */
bool DestructibleItem::is_obstacle(void) {
  return properties[type].can_be_lifted;
}

/**
 * This function is called by the engine when an entity overlaps the destructible item.
 * This is a redefinition of Detector::collision(MapEntity*).
 * If the entity is the hero, we allow him to lift the item.
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void DestructibleItem::collision(MapEntity *entity_overlapping, CollisionMode collision) {

  if (entity_overlapping->is_hero()) {

    Link *link = zsdx->game->get_link();
    KeysEffect *keys_effect = zsdx->game->get_keys_effect();
    Equipment *equipment = zsdx->game->get_equipment();

    int weight = properties[type].weight;

    if (properties[type].can_be_lifted
	&& equipment->can_lift(weight)
	&& !is_being_cut
	&& keys_effect->get_action_key_effect() == KeysEffect::ACTION_KEY_NONE
	&& link->get_state() == Link::FREE) {

      keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_LIFT);
    }
  }
}

/**
 * This function is called by the engine when a sprite overlaps the destructible item.
 * This is a redefinition of Detector::collision(Sprite*).
 * If the sprite is the sword and this item can be cut, then the item may be cut.
 * @param entity an entity
 * @param sprite_overlapping the sprite of this entity that is overlapping the detector
 */
void DestructibleItem::collision(MapEntity *entity, Sprite *sprite_overlapping) {

  if (properties[type].can_be_cut
      && !is_being_cut
      && entity->is_hero()
      && sprite_overlapping->get_animation_set_id().find("sword") != string::npos) {

    bool cut = false;
    Link *link = (Link*) entity;
    Link::State state = link->get_state();
    int animation_direction = link->get_animation_direction();

    if (state == Link::SPIN_ATTACK) {
      cut = true;
    }
    else if (state == Link::SWORD_SWINGING
	     || (is_obstacle() && link->is_moving_towards(animation_direction))) {

      SDL_Rect facing_point = link->get_facing_point();

      int distance = is_obstacle() ? 14 : 4;

      switch (animation_direction) {

      case 0:
	cut = facing_point.y >= position_in_map.y
	  && facing_point.y < position_in_map.y + position_in_map.h
	  && facing_point.x >= position_in_map.x - distance
	  && facing_point.x < position_in_map.x + position_in_map.w - distance;
	break;

      case 1:
	cut = facing_point.x >= position_in_map.x
	  && facing_point.x < position_in_map.x + position_in_map.w
	  && facing_point.y >= position_in_map.y + distance
	  && facing_point.y < position_in_map.y + position_in_map.h + distance;
	break;

      case 2:
	cut = facing_point.y >= position_in_map.y
	  && facing_point.y < position_in_map.y + position_in_map.h
	  && facing_point.x >= position_in_map.x + distance
	  && facing_point.x < position_in_map.x + position_in_map.w + distance;
	break;

      case 3:
	cut = facing_point.x >= position_in_map.x
	  && facing_point.x < position_in_map.x + position_in_map.w
	  && facing_point.y >= position_in_map.y - distance
	  && facing_point.y < position_in_map.y + position_in_map.h - distance;
	break;

      default:
	DIE("Invalid animation direction of Link: " << link->get_animation_direction());
	break;
      }
    }

    if (cut) {
      get_destruction_sound()->play();
      get_sprite()->set_current_animation("destroy");
      set_obstacle(OBSTACLE_NONE);
      is_being_cut = true;

      map->get_entities()->bring_to_front(this); // show animation destroy to front

      if (pickable_item != PickableItem::NONE) {
	bool will_disappear = (pickable_item <= PickableItem::ARROW_10);
	map->get_entities()->add_pickable_item(get_layer(), get_x(), get_y(), pickable_item,
					       pickable_item_savegame_variable, MovementFalling::MEDIUM, will_disappear);
      }
    }
  }
}

/**
 * This function is called when the player presses the action key
 * when Link is facing this detector, and the action icon lets him do this.
 * Link lifts the item if possible.
 */
void DestructibleItem::action_key_pressed(void) {

  KeysEffect *keys_effect = zsdx->game->get_keys_effect();
  Link *link = zsdx->game->get_link();

  if (keys_effect->get_action_key_effect() == KeysEffect::ACTION_KEY_LIFT
      && properties[type].can_be_lifted
      && !is_being_cut) {

    link->start_lifting(this);

    // play the sound
    ResourceManager::get_sound("lift")->play();

    // create the pickable item
    if (pickable_item != PickableItem::NONE) {
      bool will_disappear = (pickable_item <= PickableItem::ARROW_10);
      map->get_entities()->add_pickable_item(get_layer(), get_x(), get_y(), pickable_item,
					     pickable_item_savegame_variable, MovementFalling::MEDIUM, will_disappear);
    }

    // remove the item from the map
    map->get_entities()->remove_destructible_item(this);
  }
}

/**
 * Updates the item.
 */
void DestructibleItem::update(void) {

  MapEntity::update();

  if (is_being_cut && get_sprite()->is_over()) {

    // remove the item from the map
    map->get_entities()->remove_destructible_item(this);
  }
}
