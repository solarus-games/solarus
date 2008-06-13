/**
 * Some definition of types.
 * This header is included by Common.h and every class header file should include Common.h.
 */

#ifndef ZSDX_TYPES_H
#define ZSDX_TYPES_H

/**
 * Types of the id of each resource.
 */
typedef Uint32 MapId;
typedef Uint32 TilesetId;
typedef std::string MusicId;
typedef std::string SoundId;
typedef std::string SpriteAnimationsId;

/**
 * Declaration of all classes to avoid dependencies between the header files.
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
class MagicBar;
class ActionIcon;
class SwordIcon;
class PauseIcon;

class Map;
class Tileset;
class Music;
class Sound;

class Sprite;
class SpriteAnimations;
class SpriteAnimation;
class SpriteAnimationDirection;
class AnimationListener;

class Tile;
class SimpleTile;
class AnimatedTile;

class Movement;
class Movement8ByPlayer;
class MovementWithCollision;
class MovementWithSmoothCollision;

class MapEntity;
class Link;
class EntityDetector;
class TileOnMap;
class MapExit;
class MapEntrance;
class PickableItem;

#endif
