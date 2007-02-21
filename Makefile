all:
	cd src && make all && cd .. && cd testsuite && make executables && cd ..

check: all
	cd testsuite; make check; cd ..

clean:
	rm -f *~ bin/*~ include/*~
	cd src; make clean; cd ..
	cd testsuite; make clean; cd ..
