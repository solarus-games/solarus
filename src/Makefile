include ../Makedefs

objects =	ZSDX.o				\
		Game.o				\
		GameResource.o			\
		Map.o				\
		Moving.o			\
		MovingWithCollision.o		\
		Moving8ByPlayer.o		\
		AnimatedSprite.o		\
		SpriteAnimations.o		\
		SpriteAnimation.o		\
		SpriteAnimationDirection.o	\
		SimpleTile.o			\
		AnimatedTile.o			\
		TileOnMap.o			\
		TileAnimationManager.o		\
		Color.o				\
		Link.o				\
		LinkAnimations.o		\
		Tileset.o			\
		tilesets/TilesetHouse.o		\
		maps/MapLinkHouse.o		\
		Music.o				

headers = 	$(INCLUDE_DIR)/ZSDX.h				\
		$(INCLUDE_DIR)/GameResource.h			\
		$(INCLUDE_DIR)/Game.h				\
		$(INCLUDE_DIR)/Map.h				\
		$(INCLUDE_DIR)/MapEntity.h			\
		$(INCLUDE_DIR)/Moving.h				\
		$(INCLUDE_DIR)/MovingWithCollision.h		\
		$(INCLUDE_DIR)/Moving8ByPlayer.h		\
		$(INCLUDE_DIR)/AnimatedSprite.h			\
		$(INCLUDE_DIR)/SpriteAnimations.h		\
		$(INCLUDE_DIR)/SpriteAnimation.h		\
		$(INCLUDE_DIR)/SpriteAnimationDirection.h	\
		$(INCLUDE_DIR)/TileOnMap.h			\
		$(INCLUDE_DIR)/Tile.h				\
		$(INCLUDE_DIR)/SimpleTile.h			\
		$(INCLUDE_DIR)/AnimatedTile.h			\
		$(INCLUDE_DIR)/TileAnimationManager.h		\
		$(INCLUDE_DIR)/Color.h				\
		$(INCLUDE_DIR)/Link.h				\
		$(INCLUDE_DIR)/LinkAnimations.h			\
		$(INCLUDE_DIR)/Tileset.h			\
		$(INCLUDE_DIR)/tilesets/TilesetHouse.h		\
		$(INCLUDE_DIR)/maps/MapLinkHouse.h			\
		$(INCLUDE_DIR)/Music.h				

#executables = $(BIN_DIR)/tileset_editor

#all: $(executables)

#$(BIN_DIR)/tileset_editor: tileset_editor.o simple_tile_image.o animated_tile_image.o color.o
#	$(CC) -o $@ $(CPPFLAGS) $(LDFLAGS) $(objects) $(LIBS)

all: $(objects)

%.o: %.cpp $(headers)
	$(CC) -o $@ -c $< $(CPPFLAGS)

clean:
	rm -f *.o *~
