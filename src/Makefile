include ../Makedefs

objects =	zsdx.o				\
		game.o				\
		game_resource.o			\
		map.o				\
		movable.o			\
		movable_8_by_player.o		\
		animated_sprite.o		\
		sprite_animations.o		\
		sprite_animation.o		\
		sprite_animation_direction.o	\
		tile.o				\
		simple_tile.o			\
		extensible_tile.o		\
		animated_tile.o			\
		tile_animation_manager.o	\
		color.o				\
		tileset_editor.o		\
		link.o				\
		link_animations.o		\
		tileset.o			\
		tilesets/house.o		\
		maps/link_house.o

headers = 	$(INCLUDE_DIR)/zsdx.h				\
		$(INCLUDE_DIR)/datatypes.h			\
		$(INCLUDE_DIR)/game_resource.h			\
		$(INCLUDE_DIR)/game.h				\
		$(INCLUDE_DIR)/dynamic_array.h			\
		$(INCLUDE_DIR)/map.h				\
		$(INCLUDE_DIR)/map_entity.h			\
		$(INCLUDE_DIR)/movable.h			\
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
		$(INCLUDE_DIR)/tile_animation_manager.h		\
		$(INCLUDE_DIR)/color.h				\
		$(INCLUDE_DIR)/tileset_editor.h			\
		$(INCLUDE_DIR)/link.h				\
		$(INCLUDE_DIR)/link_animations.h		\
		$(INCLUDE_DIR)/tileset.h			\
		$(INCLUDE_DIR)/tilesets/house.h			\
		$(INCLUDE_DIR)/maps/link_house.h		

# executable = $(BIN_DIR)/zsdx

# all: $(executable)

# $(executable): $(objects)
# 	$(CC) $(LDFLAGS) $(CPPFLAGS) $(objects) $(LIBS) -o $(executable)

all: $(objects)

%.o: %.cpp $(headers)
	$(CC) -o $@ -c $< $(CPPFLAGS)

clean:
	rm -f *.o *~
