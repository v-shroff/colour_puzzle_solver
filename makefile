CXX = g++
FLAGS = -pedantic -Wall -Werror -O3

build/sorter: src/sorter.cpp
	$(CXX) $(FLAGS) $< -o $@

/solverWebsite/web.js: src/web.cpp
	em++ $(FLAGS) --bind -sMODULARIZE=1 -sEXPORT_ES6=1 -sALLOW_MEMORY_GROWTH=1 -sASSERTIONS=2 $< -o $@
clean:
	rm -rf build/*
