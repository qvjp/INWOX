ARCH := i686
BUILD_DIR = $(TO_ROOT)/build
CC := $(ARCH)-elf-g++

CXXFLAGS ?= -O2 -g
# 没有运行时支持，所以-fno-rtti、-fno-exceptions
CXXFLAGS += -std=gnu++14 -ffreestanding -fno-exceptions -fno-rtti -Wall -Wextra
CXXFLAGS += -I $(TO_ROOT)/kernel/include
LDFLAGS ?= -T $(TO_ROOT)/kernel/linker.ld -ffreestanding -nostdlib
LIBS ?= -lgcc

MKRESCUE ?= grub-mkrescue

ISO ?= INWOX.iso
