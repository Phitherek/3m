all:
	g++ -o 3m 3m.cpp
	make -C filetesters
debug:
	g++ -o 3m 3m.cpp -g
	make -C filetesters debug
clean:
	rm 3m
	make -C filetesters clean
cleantemp:
	rm *~
	make -C filetesters cleantemp
3m:
	g++ -o 3m 3m.cpp
3mdebug:
	g++ -o 3m 3m.cpp -g
3mclean:
	rm 3m
3mcleantemp:
	rm *~
filetesters:
	make -C filetesters
ftdebug:
	make -C filetesters debug
ftclean:
	make -C filetesters clean
ftcleantemp:
	make -C filetesters cleantemp
