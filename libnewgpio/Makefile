# This is the only thing to change in this Makefile
# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
# You NEED to and MUST change TOOL_BIN_DIR to the "bin" directory of your OpenWrt uClibc toolchain
TOOL_BIN_DIR=/home/kit/OmegaCross/OpenWrt-Toolchain-ar71xx-generic_gcc-4.8-linaro_uClibc-0.9.33.2.Linux-x86_64/toolchain-mips_34kc_gcc-4.8-linaro_uClibc-0.9.33.2/bin
# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

SHELL := /bin/bash

# C++ compiler
CC=$(TOOL_BIN_DIR)/mips-openwrt-linux-uclibc-g++
CCFLAGS=-c -Os -s -fPIC -std=gnu++11
INC=-I./hdr

# Archiver for .a library
AR=$(TOOL_BIN_DIR)/mips-openwrt-linux-uclibc-ar
ARFLAGS=-rv

# Ranlib for .a library
RANLIB=$(TOOL_BIN_DIR)/mips-openwrt-linux-uclibc-ranlib
RANLIBFLAGS=

# Flags for linking .so library
LDFLAGS=-dynamic -shared -fPIC

# Source files
SOURCES=$(shell find ./src/ -name "*.cpp" | sed s:./src/:: | tr "\n" " ")
OBJECTS=$(patsubst %.cpp,build/%.o,$(SOURCES))

# Main build targets
TARGET_BASE=libnewgpio
TARGET_A=bin/static/$(TARGET_BASE).a
TARGET_SO=bin/dynamic/$(TARGET_BASE).so

# Default target - build everything
all: static dynamic

# Build just static .a library
static: precomp $(TARGET_A)

#Build just dynamic .so library
dynamic: precomp $(TARGET_SO)

# Create and process .a library
$(TARGET_A): $(OBJECTS)
	$(AR) $(ARFLAGS) $(TARGET_A) $(OBJECTS)
	$(RANLIB) $(TARGET_A)

# Create .so library
$(TARGET_SO): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(TARGET_SO) $(OBJECTS)
	
# Compile sources 
build/%.o: ./src/%.cpp
	$(CC) $(CCFLAGS) $(INC) $< -o $@

# Remove all created files
clean-static:
	rm -rf bin/static

clean-dynamic:
	rm -rf bin/dynamic

clean: clean-static clean-dynamic
	rm -rf build/*
	rm -rf bin/*

# Ensure required sub-directories are created
precomp:
	@mkdir -p build
	@mkdir -p bin
	@mkdir -p bin/static
	@mkdir -p bin/dynamic
