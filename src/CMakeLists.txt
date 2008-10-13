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
  DialogBox.cpp
  Message.cpp

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
  SpriteAnimations.cpp
  SpriteAnimation.cpp
  SpriteAnimationDirection.cpp

  # transition effects
  Transition.cpp
  TransitionFade.cpp
  TransitionImmediate.cpp

  # map entities
  entities/MapEntities.cpp
  entities/MapEntity.cpp
  entities/Link.cpp
  entities/LinkKeys.cpp
  entities/LinkAnimations.cpp
  entities/LinkState.cpp
  entities/TileOnMap.cpp
  entities/EntityDetector.cpp
  entities/Entrance.cpp
  entities/Exit.cpp
  entities/PickableItem.cpp
  entities/PickableItemHeart.cpp
  entities/PickableItemFairy.cpp
  entities/CarriedItem.cpp
  entities/TransportableItem.cpp
  entities/Chest.cpp

  # tiles
  entities/Tileset.cpp
  entities/Tile.cpp
  entities/SimpleTile.cpp
  entities/AnimatedTile.cpp

  # movements
  movements/Movement.cpp
  movements/MovementWithCollision.cpp
  movements/MovementWithSmoothCollision.cpp
  movements/Movement8ByPlayer.cpp
  movements/MovementFalling.cpp
  movements/PathMovement.cpp
  movements/FairyMovement.cpp
  movements/FollowMovement.cpp
  movements/ThrowItemMovement.cpp
  movements/TargetMovement.cpp

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
)

INSTALL(TARGETS	zsdx
	RUNTIME DESTINATION bin
)
