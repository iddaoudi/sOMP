# SOMP Makefile

ifeq ($(shell pkg-config --libs simgrid 2> /dev/null),)
  $(error please install simgrid and make sure that its lib/pkgconfig/ path is in the PKG_CONFIG_PATH environment variable.)
endif

ifeq ($(shell pkg-config --libs tinyxml2 2> /dev/null),)
  $(error please install tinyxml2 and make sure that its lib/pkgconfig/ path is in the PKG_CONFIG_PATH environment variable.)
endif

# Compiler
CC = clang++

# Flags
CFLAGS  = -std=c++17 -O3 -Wall -ggdb \
	  $(shell pkg-config --cflags simgrid) \
	  $(shell pkg-config --cflags tinyxml2)

LIBS = $(shell pkg-config --libs simgrid) $(shell pkg-config --libs tinyxml2)

MAIN          = src/main
TARGET        = somp

all: $(TARGET)

$(TARGET): $(MAIN).cpp $(wildcard src/headers/*.hpp src/headers/models/*.hpp)
	$(CC) $(CFLAGS) -o $(TARGET) $(MAIN).cpp $(LIBS)

clean:
	$(RM) $(TARGET)
