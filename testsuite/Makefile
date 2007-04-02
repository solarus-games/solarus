include ../Makedefs

executables = dynamic_array_test game_test #tile_test tileset_editor_test
headers =	$(INCLUDE_DIR)/zsdx.h				\
		$(INCLUDE_DIR)/dynamic_array.h			\
		$(INCLUDE_DIR)/game.h				\
		$(INCLUDE_DIR)/game_resource.h			\
		$(INCLUDE_DIR)/map.h				\
		$(INCLUDE_DIR)/map_entity.h			\
		$(INCLUDE_DIR)/movable.h			\
		$(INCLUDE_DIR)/movable_with_collision.h		\
		$(INCLUDE_DIR)/movable_8_by_player.h		\
		$(INCLUDE_DIR)/animated_sprite.h		\
		$(INCLUDE_DIR)/sprite_animations.h		\
		$(INCLUDE_DIR)/sprite_animation.h		\
		$(INCLUDE_DIR)/sprite_animation_direction.h	\
		$(INCLUDE_DIR)/tile.h				\
		$(INCLUDE_DIR)/tile_data.h			\
		$(INCLUDE_DIR)/simple_tile.h			\
		$(INCLUDE_DIR)/extensible_tile.h		\
		$(INCLUDE_DIR)/animated_tile.h			\
		$(INCLUDE_DIR)/tile_animation_manager.h	\
		$(INCLUDE_DIR)/color.h				\
		$(INCLUDE_DIR)/tileset_editor.h			\
		$(INCLUDE_DIR)/link.h				\
		$(INCLUDE_DIR)/link_animations.h		\
		$(INCLUDE_DIR)/tileset.h			\
		$(INCLUDE_DIR)/tilesets/house.h			\
		$(INCLUDE_DIR)/maps/link_house.h		\
		$(INCLUDE_DIR)/music.h			

executables: $(executables) $(headers)

check: executables
	./dynamic_array_test

dynamic_array_test: dynamic_array_test.o
	$(CC) $(LDFLAGS) $(CPPFLAGS) $^ $(LIBS) -o $@

game_test: 	game_test.o				\
		$(SRC_DIR)/zsdx.o			\
		$(SRC_DIR)/game.o			\
		$(SRC_DIR)/game_resource.o		\
		$(SRC_DIR)/tile_animation_manager.o	\
		$(SRC_DIR)/color.o			\
		$(SRC_DIR)/map.o			\
		$(SRC_DIR)/movable.o			\
		$(SRC_DIR)/movable_with_collision.o	\
		$(SRC_DIR)/movable_8_by_player.o	\
		$(SRC_DIR)/animated_sprite.o		\
		$(SRC_DIR)/sprite_animations.o		\
		$(SRC_DIR)/sprite_animation.o		\
		$(SRC_DIR)/sprite_animation_direction.o	\
		$(SRC_DIR)/tile.o			\
		$(SRC_DIR)/simple_tile.o		\
		$(SRC_DIR)/extensible_tile.o		\
		$(SRC_DIR)/animated_tile.o		\
		$(SRC_DIR)/link.o			\
		$(SRC_DIR)/link_animations.o		\
		$(SRC_DIR)/tileset.o			\
		$(SRC_DIR)/tilesets/house.o		\
		$(SRC_DIR)/maps/link_house.o		\
		$(SRC_DIR)/music.o			
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
