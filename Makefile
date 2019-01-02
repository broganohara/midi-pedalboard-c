CC := g++
LD_FLAGS := -lrtmidi -lpigpio
INCLUDES := -I/usr/include/rtmidi

SRC := src
OBJ := obj

SOURCES := $(wildcard $(SRC)/*.cpp)
OBJECTS := $(patsubst $(SRC)/%.cpp, $(OBJ)/%.o,$(SOURCES))

pedal-main: $(OBJ)/midiout.o
	$(CC) $(LD_FLAGS) -o $@ $^

$(OBJ)/%.o: $(SRC)/%.cpp
	$(CC) $(INCLUDES) -I$(SRC) -c $< -o $@

clean:
	rm -f $(OBJECTS) pedal-main
