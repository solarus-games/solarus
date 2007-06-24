include ../Makedefs

objects =	zsdx.o				\
		game.o				\
		game_resource.o			\
		map.o				\
		moving.o			\
		moving_with_collision.o		\
		moving_8_by_player.o		\
		animated_sprite.o		\
		sprite_animations.o		\
		sprite_animation.o		\
		sprite_animation_direction.o	\
		simple_tile_image.o		\
		animated_tile_image.o		\
		tile.o				\
		tile_animation_manager.o	\
		color.o				\
#		tileset_editor.o		\
		link.o				\
		link_animations.o		\
		tileset.o			\
		tilesets/TilesetHouse.o		\
		maps/link_house.o		\
		music.o				

headers = 	$(INCLUDE_DIR)/zsdx.h				\
		$(INCLUDE_DIR)/game_resource.h			\
		$(INCLUDE_DIR)/game.h				\
		$(INCLUDE_DIR)/map.h				\
		$(INCLUDE_DIR)/map_entity.h			\
		$(INCLUDE_DIR)/moving.h				\
		$(INCLUDE_DIR)/moving_with_collision.h		\
		$(INCLUDE_DIR)/moving_8_by_player.h		\
		$(INCLUDE_DIR)/animated_sprite.h		\
		$(INCLUDE_DIR)/sprite_animations.h		\
		$(INCLUDE_DIR)/sprite_animation.h		\
		$(INCLUDE_DIR)/sprite_animation_direction.h	\
		$(INCLUDE_DIR)/tile.h				\
		$(INCLUDE_DIR)/tile_image.h			\
		$(INCLUDE_DIR)/simple_tile_image.h		\
		$(INCLUDE_DIR)/animated_tile_image.h		\
		$(INCLUDE_DIR)/tile_animation_manager.h		\
		$(INCLUDE_DIR)/color.h				\
#		$(INCLUDE_DIR)/tileset_editor.h			\
		$(INCLUDE_DIR)/link.h				\
		$(INCLUDE_DIR)/link_animations.h		\
		$(INCLUDE_DIR)/tileset.h			\
		$(INCLUDE_DIR)/tilesets/TilesetHouse.h		\
		$(INCLUDE_DIR)/maps/link_house.h		\
		$(INCLUDE_DIR)/music.h				

#executables = $(BIN_DIR)/tileset_editor

#all: $(executables)

#$(BIN_DIR)/tileset_editor: tileset_editor.o simple_tile_image.o animated_tile_image.o color.o
#	$(CC) -o $@ $(CPPFLAGS) $(LDFLAGS) $(objects) $(LIBS)

all: $(objects)

%.o: %.cpp $(headers)
	$(CC) -o $@ -c $< $(CPPFLAGS)

clean:
	rm -f *.o *~
