include_directories (${ZSDX_SOURCE_DIR}/include)

add_executable (zsdx

  # main classes
  ZSDX.cpp
  VideoManager.cpp
  Screen.cpp
  ResourceManager.cpp
  KeysEffect.cpp
  FileTools.cpp
  Random.cpp

  # menus
  menus/TitleScreen.cpp
  menus/SelectionMenuPhase.cpp
  menus/SelectionMenuCommon.cpp
  menus/SelectionMenuSelectFile.cpp
  menus/SelectionMenuEraseFile.cpp
  menus/SelectionMenuConfirmErase.cpp
  menus/SelectionMenuChooseName.cpp
  menus/SelectionMenuChooseMode.cpp
  menus/PauseMenu.cpp
  menus/PauseSubmenu.cpp
  menus/PauseSubmenuInventory.cpp
  menus/PauseSubmenuMap.cpp
  menus/PauseSubmenuQuestStatus.cpp
  menus/PauseSubmenuOptions.cpp

  # game
  Game.cpp
  Savegame.cpp
  Equipment.cpp
  DungeonEquipment.cpp
  Controls.cpp
  Dungeon.cpp
  Treasure.cpp
  InventoryItem.cpp
  Map.cpp
  MapLoader.cpp
  MapScript.cpp
  DialogBox.cpp
  Message.cpp
  Timer.cpp
  GameoverSequence.cpp
  Camera.cpp

  # music and sound
  Sound.cpp
  Music.cpp

  # graphics
  TextSurface.cpp
  Color.cpp
  Counter.cpp

  # HUD
  hud/HUD.cpp
  hud/HudElement.cpp
  hud/ActionIcon.cpp
  hud/SwordIcon.cpp
  hud/PauseIcon.cpp
  hud/ItemIcon.cpp
  hud/HeartsView.cpp
  hud/RupeesCounter.cpp
  hud/MagicBar.cpp
  hud/SmallKeysCounter.cpp
  hud/FloorView.cpp

  # sprites
  Sprite.cpp
  SpriteAnimationSet.cpp
  SpriteAnimation.cpp
  SpriteAnimationDirection.cpp
  PixelBits.cpp

  # transition effects
  Transition.cpp
  TransitionFade.cpp
  TransitionImmediate.cpp

  # map entities
  entities/MapEntities.cpp
  entities/MapEntity.cpp
  entities/Hero.cpp
  entities/HeroKeys.cpp
  entities/HeroAnimations.cpp
  entities/HeroState.cpp
  entities/TileOnMap.cpp
  entities/Detector.cpp
  entities/Teletransporter.cpp
  entities/DestinationPoint.cpp
  entities/PickableItem.cpp
  entities/PickableItemHeart.cpp
  entities/PickableItemFairy.cpp
  entities/CarriedItem.cpp
  entities/DestructibleItem.cpp
  entities/Chest.cpp
  entities/JumpSensor.cpp
  entities/Enemy.cpp

  # tiles
  entities/Tileset.cpp
  entities/Tile.cpp
  entities/SimpleTile.cpp
  entities/AnimatedTile.cpp

  # movements
  movements/Movement.cpp
  movements/MovementWithCollision.cpp
  movements/MovementWithSmoothCollision.cpp
  movements/PlayerMovement.cpp
  movements/FallingOnFloorMovement.cpp
  movements/PathMovement.cpp
  movements/FairyMovement.cpp
  movements/FollowMovement.cpp
  movements/ThrownItemMovement.cpp
  movements/TargetMovement.cpp
  movements/JumpMovement.cpp
  movements/StraightMovement.cpp

  # enemies
  enemies/SimpleGreenSoldier.cpp
)

if (NOT WIN32)
   SET_TARGET_PROPERTIES(zsdx PROPERTIES
			      COMPILE_FLAGS "-g -ansi -Wall -Werror")
endif (NOT WIN32)

target_link_libraries (zsdx
		      SDL
		      SDLmain
		      SDL_image
		      SDL_ttf
		      SDL_Config
		      fmodex
		      lua5.1
)

INSTALL(TARGETS	zsdx
	RUNTIME DESTINATION bin
)
