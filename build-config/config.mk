ARCH := i686
BUILD_DIR = $(TO_ROOT)/build
GCC := $(ARCH)-elf-g++

CXXFLAGS ?= -O2 -g
CXXFLAGS += -std=gnu++14 -ffreestanding -fno-exceptions -fno-rtti -Wall -Wextra
LDFLAGS ?= -T $(TO_ROOT)/kernel/linker.ld -ffreestanding -nostdlib
LIBS ?= -lgcc

MKRESCUE ?= grub-mkrescue

ISO ?= INWOX.iso
