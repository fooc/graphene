SYS ?= $(shell gcc -dumpmachine)

CC	= gcc
CFLAGS	= -Wall -O2 -std=gnu99 -fgnu89-inline -fno-builtin -nostdlib \
	  -I../include/pal -I../lib \
	  -fno-omit-frame-pointer \
	  -fno-stack-protector -fno-builtin

default: all
include ../src/Makefile.Test

lib_files = lib_timing.c lib_confidence.c sqrt.c
executables = $(patsubst %.c,%,$(filter-out $(lib_files),$(wildcard *.c)))
manifests = manifest

target = $(executables) $(manifests)

graphene_lib = .lib/graphene-lib.a
pal_lib = ../src/libpal.so
headers = $(wildcard ../include/pal/*.h) $(wildcard *.h)

all:	pal_loader $(call expand_target,$(target))

ifeq ($(DEBUG),1)
CC += -g
endif
export DEBUG

pal_loader:
	ln -sf ../../Runtime/pal_loader

manifest: manifest.template
	cp -f $< $@

%.manifest: %.manifest.template
	cp -f $< $@

ifeq ($(SYS),x86_64-linux-gnu)
$(executables): %: %.c $(patsubst %.c,%.o,$(lib_files)) ../src/user_start.o $(graphene_lib) $(pal_lib) $(headers)
	@echo [ $@ ]
	@$(CC) $(CFLAGS) $(filter-out $(headers),$^) -o $@

%.o: %.c $(headers)
	@echo [ $@ ]
	@$(CC) $(CFLAGS) -c $< -o $@

$(graphene_lib):
	$(MAKE) -C ../lib target=$(shell pwd)/.lib/

.PHONY: pack
pack: $(executables)
	@../../Scripts/pack_binaries.sh test $^
else
$(executables): .packed/benchmark.tar.gz
	tar -xmozf $< $@
endif

test: all
	env LOADER=./pal_loader ./run-tests

clean:
	rm -rf $(patsubst %.c,%.o,$(lib_files))
	rm -rf pal_loader $(call expand_target,$(target)) .lib *.cached
