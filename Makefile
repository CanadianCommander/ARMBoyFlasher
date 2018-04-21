CXX = g++
CXX_FLAGS = -std=c++14
SRC_FILES = src/*.cpp


abFlasher.a : $(SRC_FILES)
	$(CXX) $(CXX_FLAGS) -o abFlasher.a $^
