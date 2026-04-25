#**************************************************************************************************
#
#   raylib makefile for Desktop platforms (Fixed for Multi-file C++ Project)
#
#**************************************************************************************************

.PHONY: all clean

# Project Name
PROJECT_NAME       ?= main
RAYLIB_VERSION     ?= 4.5.0
RAYLIB_PATH        ?= C:/raylib/raylib

# Define compiler path on Windows
COMPILER_PATH      ?= C:/raylib/w64devkit/bin

# PLATFORM_DESKTOP
PLATFORM           ?= PLATFORM_DESKTOP

ifeq ($(OS),Windows_NT)
    PLATFORM_OS=WINDOWS
    export PATH := $(COMPILER_PATH):$(PATH)
endif

# Define raylib release directory for compiled library.
RAYLIB_RELEASE_PATH     ?= $(RAYLIB_PATH)/src

# Use g++ for C++ project
CC = g++

# Ensure Windows executables have .exe extension
ifeq ($(PLATFORM_OS),WINDOWS)
    EXT = .exe
endif

# Compiler flags: Added -std=c++17 for structured bindings and inline variables
CFLAGS += -Wall -std=c++17 -D_DEFAULT_SOURCE -Wno-missing-braces

# Build mode: DEBUG or RELEASE
BUILD_MODE ?= DEBUG

ifeq ($(BUILD_MODE),DEBUG)
    CFLAGS += -g -O0
else
    CFLAGS += -s -O1
endif

# Include paths
INCLUDE_PATHS = -I. -I$(RAYLIB_PATH)/src -I$(RAYLIB_PATH)/src/external -Iinclude -Isrc

# Library paths
LDFLAGS = -L. -L$(RAYLIB_RELEASE_PATH) -L$(RAYLIB_PATH)/src

# Libraries to link
ifeq ($(PLATFORM_OS),WINDOWS)
    LDLIBS = -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows
endif

# **********************************************************
# SOURCE FILE DETECTION (The important part!)
# **********************************************************
# This finds every .cpp file in your src directory
SRC = $(wildcard src/*.cpp)
OBJS = $(SRC:.cpp=.o)

# Default target
all: $(PROJECT_NAME)

# Link the project
$(PROJECT_NAME): $(OBJS)
	$(CC) -o $(PROJECT_NAME)$(EXT) $(OBJS) $(CFLAGS) $(INCLUDE_PATHS) $(LDFLAGS) $(LDLIBS)

# Compile .cpp files to .o files
src/%.o: src/%.cpp
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE_PATHS)

# Clean everything
clean:
ifeq ($(PLATFORM_OS),WINDOWS)
	del src\*.o *.exe /s
else
	rm -fv src/*.o $(PROJECT_NAME)
endif
	@echo Cleaning done