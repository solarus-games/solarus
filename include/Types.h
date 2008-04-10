/**
 * Some definition of types.
 * This header is included by Common.h and every file should include Common.h.
 */

#ifndef ZSDX_TYPES_H
#define ZSDX_TYPES_H

/**
 * Obstacle property for the tiles or the active objects.
 */
enum Obstacle {
  OBSTACLE_NONE,         // the entity is not an obstacle
  OBSTACLE,              // the entity is entirely an obstacle
  OBSTACLE_TOP_RIGHT,    // the upper-right half of the entity is an obstacle
  OBSTACLE_TOP_LEFT,     // the upper-left half of the entity is an obstacle
  OBSTACLE_BOTTOM_LEFT,  // the lower-left half of the entity is an obstacle
  OBSTACLE_BOTTOM_RIGHT, // the lower-right half of the entity is an obstacle
};

/**
 * Layer of a tile or an active object.
 */
enum Layer {
  LAYER_LOW,             // the entity is always below Link (floor, walls, chests,
                         // enemies and 99% of the tiles and active objects)
  LAYER_INTERMEDIATE,    // Link can be below or above the entity (platforms or objects on a plaform)
  LAYER_HIGH,            // the entity is always above Link (trees, top of doors...)
  LAYER_NB               // number of layers
};

/**
 * Type of transition between two maps.
 */
enum TransitionType {
  TRANSITION_IMMEDIATE,  // no transition between the two maps
  TRANSITION_FADE        // fade in and fade out
};

/**
 * Direction of the transition.
 */
enum TransitionDirection {
  TRANSITION_IN,
  TRANSITION_OUT
};

/**
 * Constants to identify each kind of map entity.
 */
enum EntityType {
  ENTITY_TILE = 0,
  ENTITY_ENTRANCE = 1,
  ENTITY_EXIT = 2,
};

/**
 * Types of the id of each resource.
 */
typedef Uint32 MapId;
typedef Uint32 TilesetId;
typedef std::string MusicId;
typedef std::string SoundId;
typedef std::string SpriteId;

/**
 * Declaration of some classes to avoid dependencies between the header files.
 */
class ZSDX;
class Game;
class GameResource;
class KeysEffect;
class TextDisplayed;
class Savegame;
class Equipment;
class SelectionMenu;
class TransitionEffect;

class HUD;
class HudElement;
class CounterView;
class HeartsView;
class RupeesView;
class ActionIcon;
class SwordIcon;
class PauseIcon;

class Map;
class Tileset;
class Music;
class Sound;

class AnimatedSprite;
class SpriteAnimations;
class SpriteAnimation;
class SpriteAnimationDirection;
class AnimationListener;

class Tile;
class SimpleTile;
class AnimatedTile;

class MapEntity;
class Link;
class MovingWithCollision;
class EntityDetector;
class TileOnMap;
class MapExit;
class MapEntrance;

#endif
