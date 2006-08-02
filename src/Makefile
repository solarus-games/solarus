include ../Makedefs

objects =	main.o				\
		map.o				\
	 	map_object.o			\
		tile_data.o			\
		animation_frame_handler.o	

headers = 	$(INCLUDE_DIR)/datatypes.h		\
		$(INCLUDE_DIR)/global.h			\
		$(INCLUDE_DIR)/dynamic_array.h		\
		$(INCLUDE_DIR)/main.h			\
		$(INCLUDE_DIR)/map.h			\
		$(INCLUDE_DIR)/map_object.h		\
		$(INCLUDE_DIR)/tile_data.h		\
		$(INCLUDE_DIR)/animation_handler.h	

# executable = $(BIN_DIR)/zsdx

# all: $(executable)

# $(executable): $(objects)
# 	$(CC) $(LDFLAGS) $(CPPFLAGS) $(objects) $(LIBS) -o $(executable)

all: $(objects)

%.o: %.cpp $(headers)
	$(CC) -o $@ -c $< $(CPPFLAGS)

clean:
	rm -f *.o *~
