ARCH := i686
BUILD_DIR = $(TO_ROOT)/build
AR := $(ARCH)-elf-ar
CC := $(ARCH)-elf-gcc
CXX := $(ARCH)-elf-g++

# 在某些平台‘/’会被视为注释字符，也就不能在表达式中使用，--divide使‘/’变为正常字符，但不会影响
# 在行开头使用的‘/’进行注释的行为
ASFLAGS += -Wa,--divide
CXXFLAGS ?= -O0 -g
# 没有运行时支持，所以-fno-rtti、-fno-exceptions
CXXFLAGS += --sysroot=$(SYSROOT) -std=gnu++14 -ffreestanding -fno-exceptions -Wall -Wextra -fno-rtti
# CPPFLAGS中CPP是C PreProcessor的缩写
CPPFLAGS += -I $(TO_ROOT)/kernel/include -I $(SYSROOT)/usr/include
LDFLAGS ?= --sysroot=$(SYSROOT) -T $(TO_ROOT)/kernel/linker.ld -ffreestanding -nostdlib
LIBS ?= -lk -lgcc

MKRESCUE ?= grub-mkrescue

ISO ?= INWOX.iso

SYSROOT ?= $(TO_ROOT)/sysroot
