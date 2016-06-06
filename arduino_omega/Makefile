# Makefile to package arduino-omega Arduino library in a .zip file
# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

SHELL := /bin/bash

# Default target
all: prezip dozip

# Zip the library
dozip:
	zip -r ./zip/Arduino-Omega.zip ./Arduino_Omega/*

# Remove all created files
clean:
	rm -rf zip/*

# Ensure required sub-directories are created
prezip:
	@mkdir -p zip
 
