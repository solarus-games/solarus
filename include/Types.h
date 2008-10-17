/**
 * Some definition of types.
 * This header is included by Common.h and every class header file should include Common.h.
 */

#ifndef ZSDX_TYPES_H
#define ZSDX_TYPES_H

/**
 * Types of the id of each resource.
 */
typedef std::string ImageId;
typedef Uint32 MapId;
typedef Uint32 TilesetId;
typedef std::string MusicId;
typedef std::string SoundId;
typedef std::string SpriteAnimationSetId;

/**
 * Messages.
 */
typedef std::string MessageId;

/**
 * Declaration of all classes to avoid dependencies between the header files.
 */

// main classes
class ZSDX;
class VideoManager;
class Screen;
class ResourceManager;
class KeysEffect;
class FileTools;
class Random;

// menus
class TitleScreen;
class SelectionMenuPhase;
class SelectionMenuCommon;
class SelectionMenuEraseFile;
class SelectionMenuConfirmErase;
class SelectionMenuChooseName;
class SelectionMenuChooseMode;
class PauseMenu;
class PauseSubmenu;
class PauseSubmenuInventory;
class PauseSubmenuMap;
class PauseSubmenuQuestStatus;
class PauseSubmenuOptions;

// game
class Game;
class Savegame;
class Equipment;
class DungeonEquipment;
class Controls;
class Dungeon;
class Treasure;
class InventoryItem;
class Map;
class MapLoader;
class DialogBox;
class Message;

// music and sound
class Music;
class Sound;

// graphics
class Color;
class TextSurface;

// HUD
class HUD;
class HudElement;
class ActionIcon;
class SwordIcon;
class PauseIcon;
class ItemIcon;
class Counter;
class HeartsView;
class RupeesCounter;
class MagicBar;
class SmallKeysCounter;
class FloorView;

// sprites
class Sprite;
class SpriteAnimationSet;
class SpriteAnimation;
class SpriteAnimationDirection;
class AnimationListener;
class PixelBits;

// transition effects
class Transition;
class TransitionFade;
class TransitionImmediate;

// map entities
class MapEntities;
class MapEntity;
class Link;
class TileOnMap;
class Detector;
class Entrance;
class Exit;
class PickableItem;
class PickableItemHeart;
class PickableItemFairy;
class CarriedItem;
class DestructibleItem;
class Chest;

// tiles
class Tileset;
class Tile;
class SimpleTile;
class AnimatedTile;

// movements
class Movement;
class MovementWithCollision;
class MovementWithSmoothCollision;
class Movement8ByPlayer;
class MovementFalling;
class PathMovement;
class FairyMovement;
class FollowMovement;
class ThrowItemMovement;
class TargetMovement;

#endif
