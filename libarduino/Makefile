# These are the only things to change in this Makefile
# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
# You NEED to and MUST change TOOL_BIN_DIR to the "bin" directory of your OpenWrt uClibc toolchain
TOOL_BIN_DIR=/home/kit/OmegaCross/OpenWrt-Toolchain-ar71xx-generic_gcc-4.8-linaro_uClibc-0.9.33.2.Linux-x86_64/toolchain-mips_34kc_gcc-4.8-linaro_uClibc-0.9.33.2/bin
# You MAY need to change LIBNEW-I2C_DIR to relative directory of libnewi2c if you are not using the sources as originally supplied
LIBNEW-I2C_DIR=../libnewi2c
# You MAY need to change LIBNEW-GPIO_DIR to relative directory of libnewgpio if you are not using the sources as originally supplied
LIBNEW-GPIO_DIR=../libnewgpio
# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

SHELL := /bin/bash
# Use buildep=1 on make command line to also build other libraries first
builddep?=0

# C++ compiler
CC=$(TOOL_BIN_DIR)/mips-openwrt-linux-uclibc-g++
CCFLAGS=-c -Os -s -fPIC -std=gnu++11

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

# Includes and libraries
INC=-I./hdr -I$(LIBNEW-I2C_DIR)/hdr -I$(LIBNEW-GPIO_DIR)/hdr
LIBS_A=$(LIBNEW-I2C_DIR)/bin/static/libnewi2c.a $(LIBNEW-GPIO_DIR)/bin/static/libnewgpio.a
LIBS_SO=$(LIBNEW-I2C_DIR)/bin/dynamic/libnewi2c.so $(LIBNEW-GPIO_DIR)/bin/dynamic/libnewgpio.so
SYS_LIBS=-lpthread -ldl

# Main build targets
TARGET_BASE=libarduino
TARGET_A=bin/static/$(TARGET_BASE).a
TARGET_SO=bin/dynamic/$(TARGET_BASE).so

# Default target - build everything
all: static dynamic

#Build nolib .so library
ifeq ($(builddep),1)
dynamic: precomp MAKE_LIB $(TARGET_SO)
else
dynamic: precomp $(TARGET_SO)
endif

#Build withlib .so library
ifeq ($(builddep),1)
static: precomp MAKE_LIB $(TARGET_A)
else
static: precomp $(TARGET_A)
endif

# Ensure library is built
MAKE_LIB:
	pushd $(LIBNEW-I2C_DIR); make clean; make builddep=$(builddep); popd
	pushd $(LIBNEW-GPIO_DIR); make clean; make builddep=$(builddep); popd

# Create dynamic .so library
$(TARGET_SO): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(TARGET_SO) $(OBJECTS) $(LIBS_SO) $(SYS_LIBS)
	
#	$(CC) $(LDFLAGS) -o $(TARGET_SO) $(OBJECTS_SO) $(LIBS) 
#$(SYS_LIBS)
	
# Create static .a library
$(TARGET_A): $(OBJECTS)
	$(AR) $(ARFLAGS) $(TARGET_A) $(OBJECTS) $(LIBS_A)
	$(RANLIB) $(TARGET_A)
	
# Compile sources for library
build/%.o: src/%.cpp
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
