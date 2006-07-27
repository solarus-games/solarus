include ../Makedefs

objects = main.o
headers = ../include/zsdx.h
executable = $(BIN_DIR)/zsdx

all: $(executable)

$(executable): $(objects)
	$(CC) $(LDFLAGS) $(CPPFLAGS) $(objects) $(LIBS) -o $(executable)

%.o: %.cpp $(headers)
	$(CC) -o $@ -c $< $(CPPFLAGS)

clean:
	rm -f *.o *~
