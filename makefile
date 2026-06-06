CXX = g++
FLAGS = -pedantic -Wall -Werror -O3

build/sorter: src/sorter.cpp
	$(CXX) $(FLAGS) $< -o $@
