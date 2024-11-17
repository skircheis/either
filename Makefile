CXX=clang++
CXXFLAGS=-gdwarf-5 --std=c++17 -lfmt -O0
BUILD_DIR=build

run: $(BUILD_DIR)/either
	./$<

$(BUILD_DIR)/either: either.cpp either.hpp $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

$(BUILD_DIR):
	mkdir -p $@
