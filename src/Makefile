include ../Makedefs

objects = main.o map.o map_object.o
headers = $(INCLUDE_DIR)/zsdx.h $(INCLUDE_DIR)/map.h $(INCLUDE_DIR)/map_object.h #$(INCLUDE_DIR)/dynamic_array.h
executable = $(BIN_DIR)/zsdx

all: $(executable)

$(executable): $(objects)
	$(CC) $(LDFLAGS) $(CPPFLAGS) $(objects) $(LIBS) -o $(executable)

%.o: %.cpp $(headers)
	$(CC) -o $@ -c $< $(CPPFLAGS)

clean:
	rm -f *.o *~
