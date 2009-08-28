include_directories (${ZSDX_SOURCE_DIR}/include ${ZSDX_SOURCE_DIR}/include/snes_spc)

add_executable (zsdx

  # main classes
  ZSDX.cpp
  VideoManager.cpp
  Screen.cpp
  ResourceManager.cpp
  KeysEffect.cpp
  FileTools.cpp
  Random.cpp
  Geometry.cpp

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
  DebugKeys.cpp
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
  TransitionScrolling.cpp

  # map entities
  entities/MapEntities.cpp
  entities/MapEntity.cpp
  entities/Hero.cpp
  entities/HeroSprites.cpp
  entities/HeroKeys.cpp
  entities/HeroState.cpp
  entities/Tile.cpp
  entities/DynamicTile.cpp
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
  entities/InteractiveEntity.cpp
  entities/Block.cpp
  entities/Switch.cpp
  entities/CustomObstacle.cpp
  entities/Sensor.cpp
  entities/CrystalSwitch.cpp
  entities/CrystalSwitchBlock.cpp
  entities/ShopItem.cpp
  entities/Boomerang.cpp
  entities/ConveyorBelt.cpp
  entities/Door.cpp

  # tile patterns
  entities/Tileset.cpp
  entities/TilePattern.cpp
  entities/SimpleTilePattern.cpp
  entities/AnimatedTilePattern.cpp
  entities/ParallaxTilePattern.cpp

  # movements
  movements/Movement.cpp
  movements/CollisionMovement.cpp
  movements/SmoothCollisionMovement.cpp
  movements/PlayerMovement.cpp
  movements/FallingOnFloorMovement.cpp
  movements/PixelMovement.cpp
  movements/FairyMovement.cpp
  movements/FollowMovement.cpp
  movements/TargetMovement.cpp
  movements/JumpMovement.cpp
  movements/StraightMovement.cpp
  movements/PathMovement.cpp
  movements/RandomWalkMovement.cpp
  movements/PathFindingMovement.cpp
  movements/PathFinding.cpp

  # enemies
  enemies/SimpleGreenSoldier.cpp
  enemies/Bubble.cpp
  enemies/Tentacle.cpp
  enemies/PapillosaurKing.cpp

  # external source files
  snes_spc/dsp.cpp
  snes_spc/SNES_SPC.cpp
  snes_spc/SNES_SPC_misc.cpp
  snes_spc/SNES_SPC_state.cpp
  snes_spc/spc.cpp
  snes_spc/SPC_DSP.cpp
  snes_spc/SPC_Filter.cpp
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
		      SDL_mixer
		      SDL_sound
		      lua5.1
		      physfs
		      stdc++
)

INSTALL(TARGETS	zsdx
	RUNTIME DESTINATION bin
)
