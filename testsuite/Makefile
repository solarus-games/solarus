include ../Makedefs

executables = dynamic_array_test
headers = ../include/zsdx.h ../include/dynamic_array.h

executables: $(executables) $(headers)

check: executables
	./dynamic_array_test

dynamic_array_test: dynamic_array_test.o

%.o: %.cpp $(headers)
	$(CC) -o $@ -c $< $(CPPFLAGS)

clean:
	rm -f *.o *~ $(executables)
