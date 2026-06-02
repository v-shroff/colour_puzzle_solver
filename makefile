CXX = g++
FLAGS = -pedantic -Wall -Werror

build/sorter: src/sorter.cpp
	$(CXX) $(FLAGS) $< -o $@
