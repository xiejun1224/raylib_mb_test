# Linux build for the Snake game. The Visual Studio project remains the Windows build.
CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2
PKG_CONFIG ?= pkg-config

SRC := raylib_mb_test/test.cpp
BIN := raylib_mb_test/raylib_mb_test

RAYLIB_CFLAGS := $(shell $(PKG_CONFIG) --cflags raylib)
RAYLIB_LIBS := $(shell $(PKG_CONFIG) --libs raylib)

.PHONY: all run clean

all: $(BIN)

$(BIN): $(SRC)
	$(CXX) $(CXXFLAGS) $(RAYLIB_CFLAGS) -o $@ $< $(RAYLIB_LIBS)

run: $(BIN)
	cd raylib_mb_test && ./raylib_mb_test

clean:
	rm -f $(BIN)
