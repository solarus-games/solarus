include_directories (${ZSDX_SOURCE_DIR}/include)

add_executable (zsdx

  # main classes
  ZSDX.cpp
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

  # game
  Game.cpp
  Savegame.cpp
  Equipment.cpp
  Map.cpp
  MapLoader.cpp
  DialogBox.cpp
  Message.cpp

  # music and sound
  Sound.cpp
  Music.cpp

  # graphics
  TextDisplayed.cpp
  Color.cpp

  # HUD
  HUD.cpp
  HudElement.cpp
  ActionIcon.cpp
  SwordIcon.cpp
  PauseIcon.cpp
  CounterView.cpp
  HeartsView.cpp
  RupeesView.cpp
  MagicBar.cpp

  # sprites
  Sprite.cpp
  SpriteAnimations.cpp
  SpriteAnimation.cpp
  SpriteAnimationDirection.cpp

  # transition effects
  TransitionEffect.cpp
  TransitionFade.cpp
  TransitionImmediate.cpp

  # map entities
  entities/MapEntities.cpp
  entities/MapEntity.cpp
  entities/Link.cpp
  entities/TileOnMap.cpp
  entities/EntityDetector.cpp
  entities/MapEntrance.cpp
  entities/MapExit.cpp
  entities/PickableItem.cpp
  entities/PickableItemHeart.cpp
  entities/PickableItemFairy.cpp
  entities/CarriedItem.cpp
  entities/TransportableItem.cpp

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
  movements/MovementPath.cpp
  movements/FairyMovement.cpp
  movements/FollowMovement.cpp
  movements/ThrowItemMovement.cpp

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
