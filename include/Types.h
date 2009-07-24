/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
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
class Geometry;

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
class Map;
class MapLoader;
class MapScript;
class DialogBox;
class Message;
class Timer;
class GameoverSequence;
class Camera;
class InventoryItem;

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
class PixelBits;

// transition effects
class Transition;
class TransitionFade;
class TransitionImmediate;
class TransitionScrolling;

// map entities
class MapEntities;
class MapEntity;
class Hero;
class Tile;
class DynamicTile;
class Detector;
class Teletransporter;
class DestinationPoint;
class PickableItem;
class PickableItemHeart;
class PickableItemFairy;
class CarriedItem;
class DestructibleItem;
class Chest;
class JumpSensor;
class Enemy;
class InteractiveEntity;
class Block;
class Switch;
class CustomObstacle;
class Sensor;
class CrystalSwitch;
class CrystalSwitchBlock;
class ShopItem;
class Boomerang;
class ConveyorBelt;
class Door;

// tile patterns
class Tileset;
class TilePattern;
class SimpleTilePattern;
class AnimatedTilePattern;
class ParallaxTilePattern;

// movements
class Movement;
class CollisionMovement;
class SmoothCollisionMovement;
class PlayerMovement;
class FallingOnFloorMovement;
class PixelMovement;
class FairyMovement;
class FollowMovement;
class TargetMovement;
class JumpMovement;
class StraightMovement;
class PathMovement;
class RandomWalkMovement;
class PathFindingMovement;
class PathFinding;

// enemies
class SimpleGreenSoldier;
class Bubble;
class Tentacle;

#endif
