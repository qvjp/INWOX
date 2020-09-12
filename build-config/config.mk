ARCH := i686
BUILD_DIR = $(TO_ROOT)/build
SYSROOT ?= $(TO_ROOT)/sysroot
AR := $(ARCH)-inwox-ar
CC := $(ARCH)-inwox-gcc
CXX := $(ARCH)-inwox-g++

# 在某些平台‘/’会被视为注释字符，也就不能在表达式中使用，--divide使‘/’变为正常字符，但不会影响
# 在行开头使用的‘/’进行注释的行为
ASFLAGS += -Wa,--divide
CXXFLAGS ?= -O0 -g
# 没有运行时支持，所以-fno-rtti、-fno-exceptions
CXXFLAGS += --sysroot=$(SYSROOT) -std=gnu++14 -ffreestanding -fno-exceptions -Wall -Wextra -fno-rtti
CXXFLAGS += -D__USE_INWOX
# CPPFLAGS中CPP是C PreProcessor的缩写
CPPFLAGS += -I $(TO_ROOT)/kernel/include -I $(SYSROOT)/usr/include
LDFLAGS ?= --sysroot=$(SYSROOT) -T $(TO_ROOT)/kernel/linker.ld -ffreestanding -nostdlib
LIBS ?= -lk -lgcc

MKRESCUE ?= grub-mkrescue

ISO ?= INWOX.iso
INITRD ?= $(BUILD_DIR)/initrd.tar

SYSROOT ?= $(TO_ROOT)/sysroot

BIN_DIR = $(SYSROOT)/bin
INCLUDE_DIR = $(SYSROOT)/include
LIB_DIR = $(SYSROOT)/lib
