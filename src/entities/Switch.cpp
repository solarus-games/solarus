#include "entities/Switch.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "Sprite.h"
#include "Map.h"
#include "MapScript.h"

/**
 * Constructor.
 * @param name name of the entity
 * @param layer layer of the entity
 * @param x x position of the entity's rectangle
 * @param y y position of the entity's rectangle
 * @param subtype the subtype of switch
 * @param needs_block true if a block is required to enabled this switch
 * @param disable_when_leaving true to disable the switch when the hero or 
 * the block leaves it
 */
Switch::Switch(std::string name, MapEntity::Layer layer, int x, int y,
	       Subtype subtype, bool needs_block, bool disable_when_leaving):
  Detector(COLLISION_ORIGIN_POINT, name, layer, x, y, 16, 16),
  subtype(subtype), needs_block(needs_block), disable_when_leaving(disable_when_leaving),
  enabled(false) {

  if (subtype == INVISIBLE && needs_block) {
    DIE("The switch '" << name << "' is invisible but needs a block");
  }

  if (subtype == NORMAL) {
    create_sprite("entities/switch");
    get_sprite()->set_current_animation("disabled");
  }
}

/**
 * Destructor.
 */
Switch::~Switch(void) {

}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
MapEntity::EntityType Switch::get_type() {
  return SWITCH;
}

/**
 * This function is called by the engine when an entity overlaps the switch.
 * This is a redefinition of Detector::collision().
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void Switch::collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {

  if (enabled) {
    return;
  }

  if (entity_overlapping->is_hero()) {
    enabled = !needs_block;
  }
  else if (entity_overlapping->get_type() == BLOCK) {
    // don't enable an invisible switch with a block
    enabled = (subtype != INVISIBLE);
  }

  if (enabled) {

    if (subtype != INVISIBLE) {
      ResourceManager::get_sound("switch")->play();
      get_sprite()->set_current_animation("enabled");
    }

    map->get_script()->event_switch_enabled(this);
  }
}

// TODO take into account disable_when_leaving
