#include "entities/Enemy.h"
#include "ZSDX.h"
#include "Game.h"
#include "Savegame.h"
#include "enemies/SimpleGreenSoldier.h"

/**
 * Creates an enemy.
 * This constructor can only be called by the subclasses. Use the static method create() to create enemies.
 * @param params the name and position of the enemy
 */
Enemy::Enemy(const ConstructionParameters &params):
  Detector(COLLISION_RECTANGLE, params.name, params.layer, params.x, params.y, 0, 0) {

}

/**
 * Destructor.
 */
Enemy::~Enemy(void) {

}

/**
 * Creates an enemy with the specified type.
 * This method acts like a constructor, except that it can return NULL if the enemy
 * is already dead and cannot be killed again (e.g. a boss).
 * @param type type of enemy to create
 * @param name a name identifying the enemy
 * @param rank rank of the enemy: normal, miniboss or boss
 * @param savegame_variable index of the boolean variable indicating that
 * @param layer layer of the enemy on the map
 * @param x x position of the enemy on the map
 * @param y y position of the enemy on the map
 * @param direction initial direction of the enemy on the map (0 to 3)
 * this enemy is killed, or -1 if this enemy is not saved
 * @param pickable_item_type type of pickable item the enemy drops
 * @param pickable_item_savegame_variable index of the boolean variable
 */
Enemy * Enemy::create(EnemyType type, EnemyRank rank, int savegame_variable,
		      string name, Layer layer, int x, int y, int direction,
		      PickableItem::ItemType pickable_item_type, int pickable_item_savegame_variable) {

  // see if the enemy is alive
  if (savegame_variable != -1 && zsdx->game->get_savegame()->get_boolean(savegame_variable)) {
    return NULL;
  }

  // create the enemy
  Enemy *enemy;
  const ConstructionParameters params = {name, layer, x, y};

  switch (type) {
    
  case SIMPLE_GREEN_SOLDIER:
    enemy = new SimpleGreenSoldier(params);
    break;

  default:
    DIE("Unknown enemy type '" << type << "'");
  }

  // initialize the fields
  enemy->set_direction(direction);
  enemy->rank = rank;
  enemy->savegame_variable = savegame_variable;
  enemy->pickable_item_type = pickable_item_type;
  enemy->pickable_item_savegame_variable = pickable_item_savegame_variable;
  enemy->initialize();
  enemy->set_rectangle_from_sprite();

  return enemy;
}

/**
 * This function is called when the enemy collides with another entity.
 * @param entity_overlapping the other entity
 * @param collision_mode the collision mode that detected the collision
 */
void Enemy::collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {
  // TODO
}
