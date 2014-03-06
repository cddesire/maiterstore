CXX ?= g++
CC ?= gcc
CMAKE = cmake
TOP = $(shell pwd)
MAKE := $(MAKE) --no-print-directory

export CXX CC CFLAGS CPPFLAGS OPROFILE

all: release 

release: 
	@mkdir -p bin/release
	@cd bin/release && $(CMAKE) -DCMAKE_BUILD_TYPE=Release $(TOP)/src
	@cd bin/release && $(MAKE)
	@cp bin/release/examples/maiter maiter

debug: 
	@mkdir -p bin/debug
	@cd bin/debug && $(CMAKE) -DCMAKE_BUILD_TYPE=Debug $(TOP)/src
	@cd bin/debug  && $(MAKE)
	@cp bin/debug/examples/maiter maiter

docs:
	@cd docs/ && $(MAKE)

clean:
	rm -rf bin/*

.DEFAULT: bin/debug/Makefile bin/release/Makefile
	@cd bin/release && $(MAKE) $@
	@cd bin/debug && $(MAKE) $@
