# makefile

# project name (generate executable with this name)
TARGET    := TestFARSITE

CC        := g++
LD        := g++

CXXFLAGS  := -c -std=c++17 -g -Wall
LFLAGS    :=

MODULES   := main farsite5 icf fuel_moisture shapelib utils  
SRC_DIR   := $(addprefix src/,$(MODULES))
BUILD_DIR := $(addprefix build/,$(MODULES))

SRC       := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp))
OBJ       := $(patsubst src/%.cpp,build/%.o,$(SRC))
INCLUDES  := $(addprefix -I,$(SRC_DIR))

vpath %.cpp $(SRC_DIR)

define make-goal
$1/%.o: %.cpp
	$(CC) $(INCLUDES) $(CXXFLAGS) -c $$< -o $$@
endef

.PHONY: all checkdirs clean

all: checkdirs build/$(TARGET)

build/$(TARGET): $(OBJ)
	$(LD) $^ -o $(LFLAGS) $@

checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR)

$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir))))
