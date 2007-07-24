include ../Makedefs

executables = game_test #tile_test tileset_editor_test

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
		$(INCLUDE_DIR)/maps/LinkHouse.h			\
		$(INCLUDE_DIR)/Music.h				

executables: $(executables) $(headers)

check: executables


game_test: 	game_test.o				\
		$(SRC_DIR)/ZSDX.o			\
		$(SRC_DIR)/Game.o			\
		$(SRC_DIR)/GameResource.o		\
		$(SRC_DIR)/Tile.o			\
		$(SRC_DIR)/TileAnimationManager.o	\
		$(SRC_DIR)/Color.o			\
		$(SRC_DIR)/Map.o			\
		$(SRC_DIR)/Moving.o			\
		$(SRC_DIR)/MovingWithCollision.o	\
		$(SRC_DIR)/Moving8ByPlayer.o		\
		$(SRC_DIR)/AnimatedSprite.o		\
		$(SRC_DIR)/SpriteAnimations.o		\
		$(SRC_DIR)/SpriteAnimation.o		\
		$(SRC_DIR)/SpriteAnimationDirection.o	\
		$(SRC_DIR)/SimpleTile.o			\
		$(SRC_DIR)/AnimatedTile.o		\
		$(SRC_DIR)/Link.o			\
		$(SRC_DIR)/LinkAnimations.o		\
		$(SRC_DIR)/Tileset.o			\
		$(SRC_DIR)/tilesets/TilesetHouse.o	\
		$(SRC_DIR)/maps/LinkHouse.o		\
		$(SRC_DIR)/Music.o			
	$(CC) $(LDFLAGS) $(CPPFLAGS) $^ $(LIBS) -o $@

tile_test: 	tile_test.o				\
		$(SRC_DIR)/main.o			\
		$(SRC_DIR)/map.o			\
		$(SRC_DIR)/map_object.o			\
		$(SRC_DIR)/tile_data.o			\
		$(SRC_DIR)/tile_animation_manager.o	
	$(CC) $(LDFLAGS) $(CPPFLAGS) $^ $(LIBS) -o $@

tileset_editor_test:	tileset_editor_test.o		\
			$(SRC_DIR)/tileset_editor.o
	$(CC) $(LDFLAGS) $(CPPFLAGS) $^ $(LIBS) -o $@

%.o: %.cpp $(headers)
	$(CC) -o $@ -c $< $(CPPFLAGS)

clean:
	rm -f *.o *~ $(executables)
