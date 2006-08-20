include ../Makedefs

objects =	main.o				\
		game.o				\
		game_resource.o			\
		map.o				\
		movable.o				\
		tile.o				\
		tile_data.o			\
		animation_frame_handler.o	\
		color.o				\
		tileset_editor.o		\
		link.o				\
		tileset.o			\
		tilesets/house.o		\
		maps/link_house.o

headers = 	$(INCLUDE_DIR)/datatypes.h		\
		$(INCLUDE_DIR)/global.h			\
		$(INCLUDE_DIR)/game_resource.h		\
		$(INCLUDE_DIR)/game.h			\
		$(INCLUDE_DIR)/sdl_user_events.h	\
		$(INCLUDE_DIR)/dynamic_array.h		\
		$(INCLUDE_DIR)/main.h			\
		$(INCLUDE_DIR)/map.h			\
		$(INCLUDE_DIR)/map_object.h		\
		$(INCLUDE_DIR)/movable.h		\
		$(INCLUDE_DIR)/tile.h			\
		$(INCLUDE_DIR)/tile_data.h		\
		$(INCLUDE_DIR)/animation_handler.h	\
		$(INCLUDE_DIR)/color.h			\
		$(INCLUDE_DIR)/tileset_editor.h		\
		$(INCLUDE_DIR)/link.h		\
		$(INCLUDE_DIR)/tileset.h		\
		$(INCLUDE_DIR)/tilesets/house.h		\
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
