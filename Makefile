# project name (generate executable with this name)
TARGET	= TestFARSITE

CXX=g++
CXXFLAGS=-c -std=c++17 -g -Wall -DUNIX -Wno-deprecated -Wno-reorder -H
LINKER   = g++ -o
LFLAGS   =

SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin

SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
rm       = rm -f

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
  ARCH = 2
  $(info *****************  I'm a MAC  ******************)
endif
ifeq ($(UNAME), Linux)
  ARCH = 1
  $(info *****************  I'm a Linux machine  ******************)
endif



$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(LINKER) $@ $(LFLAGS) $(OBJECTS)
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONEY: clean
clean:
	@$(rm) $(OBJECTS)
	@echo "Cleanup complete!"

.PHONEY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"

