include ../Makedefs

executables = dynamic_array_test tile_test
headers = 	$(INCLUDE_DIR)/datatypes.h     \
		$(INCLUDE_DIR)/dynamic_array.h \
		$(INCLUDE_DIR)/map.h           \
		$(INCLUDE_DIR)/map_object.h    \
		$(INCLUDE_DIR)/tile_data.h     \

executables: $(executables) $(headers)

check: executables
	./dynamic_array_test

dynamic_array_test: dynamic_array_test.o
	$(CC) $(LDFLAGS) $(CPPFLAGS) $^ $(LIBS) -o $@

tile_test: tile_test.o                  \
		$(SRC_DIR)/map.o        \
		$(SRC_DIR)/map_object.o \
		$(SRC_DIR)/tile.o
	$(CC) $(LDFLAGS) $(CPPFLAGS) $^ $(LIBS) -o $@

%.o: %.cpp $(headers)
	$(CC) -o $@ -c $< $(CPPFLAGS)

clean:
	rm -f *.o *~ $(executables)
