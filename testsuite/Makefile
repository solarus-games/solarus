include ../Makedefs

executables = #./grid_test ./solver_test
headers = ../include/zsdx.h

executables: $(executables) $(headers)

check: executables
#./grid_test
##./solver_test

#grid_test: grid_test.o $(SRC_DIR)/grid.o
#solver_test: solver_test.o $(SRC_DIR)/solver.o $(SRC_DIR)/grid.o

clean:
	rm -f *.o *~ $(executables)