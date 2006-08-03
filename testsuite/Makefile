include ../Makedefs

executables = dynamic_array_test tile_test
headers = 	$(INCLUDE_DIR)/datatypes.h		\
		$(INCLUDE_DIR)/global.h			\
		$(INCLUDE_DIR)/sdl_user_events.h	\
		$(INCLUDE_DIR)/dynamic_array.h		\
		$(INCLUDE_DIR)/main.h			\
		$(INCLUDE_DIR)/map.h			\
		$(INCLUDE_DIR)/map_object.h		\
		$(INCLUDE_DIR)/tile_data.h		\
		$(INCLUDE_DIR)/animation_frame_handler.h	

executables: $(executables) $(headers)

check: executables
	./dynamic_array_test

dynamic_array_test: dynamic_array_test.o
	$(CC) $(LDFLAGS) $(CPPFLAGS) $^ $(LIBS) -o $@

tile_test: 	tile_test.o				\
		$(SRC_DIR)/main.o			\
		$(SRC_DIR)/map.o			\
		$(SRC_DIR)/map_object.o			\
		$(SRC_DIR)/tile_data.o			\
		$(SRC_DIR)/animation_frame_handler.o	
	$(CC) $(LDFLAGS) $(CPPFLAGS) $^ $(LIBS) -o $@

%.o: %.cpp $(headers)
	$(CC) -o $@ -c $< $(CPPFLAGS)

clean:
	rm -f *.o *~ $(executables)
