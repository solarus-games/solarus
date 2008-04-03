include_directories (${ZSDX_SOURCE_DIR}/include)

add_executable (zsdx
	       ZSDX.cpp
	       FileTools.cpp
	       Game.cpp
	       GameResource.cpp
	       Savegame.cpp
	       TextDisplayed.cpp
	       TitleScreen.cpp
	       SelectionMenu.cpp
	       HUD.cpp
	       HudElement.cpp
	       CounterView.cpp
	       HeartsView.cpp
	       RupeesView.cpp
	       Map.cpp
	       MapEntrance.cpp
	       MapEntity.cpp
	       Moving.cpp
	       MovingWithCollision.cpp
	       MovingWithSmoothCollision.cpp
	       Moving8ByPlayer.cpp
	       EntityDetector.cpp
	       MapExit.cpp
	       AnimatedSprite.cpp
	       SpriteAnimations.cpp
	       SpriteAnimation.cpp
	       SpriteAnimationDirection.cpp
	       SimpleTile.cpp
	       AnimatedTile.cpp
	       TileOnMap.cpp
	       Tile.cpp
	       TileAnimationManager.cpp
	       Color.cpp
	       Link.cpp
	       Music.cpp
	       Sound.cpp
	       Tileset.cpp
	       TransitionEffect.cpp
	       TransitionImmediate.cpp
	       TransitionFade.cpp
)

target_link_libraries (zsdx
		      SDL
		      SDLmain
		      SDL_image
		      SDL_ttf
		      fmodex
)
