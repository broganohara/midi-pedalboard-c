SRC_DIR := src
OBJ_DIR := obj
SRC_FILES := $(wildcard $(SRC_DIR/*.cpp))
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LD_FLAGS := -lrtmidi
INCLUDES := -I/usr/include/rtmidi

pedal-main: $(OBJ_FILES)
	g++ $(SRC_DIR)/midiout.cpp $(INCLUDES) $(LD_FLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(LD_FLAGS) $(INCLUDES) -c -o $@ $<
