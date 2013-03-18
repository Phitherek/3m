all:
	make -C filetesters
	make -C tests
debug:
	make -C filetesters debug
	make -C tests debug
clean:
	make -C filetesters clean
	make -C tests clean
cleantemp:
	rm *~
	make -C filetesters cleantemp
filetesters:
	make -C filetesters
ftdebug:
	make -C filetesters debug
ftclean:
	make -C filetesters clean
ftcleantemp:
	make -C filetesters cleantemp
tests:
	make -C tests
testsdebug:
	make -C tests debug
testsclean:
	make -C testsclean
