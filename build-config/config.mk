ARCH := i686
BUILD_DIR = $(TO_ROOT)/build
CC := $(ARCH)-elf-g++

# 在某些平台‘/’会被视为注释字符，也就不能在表达式中使用，--divide使‘/’变为正常字符，但不会影响
# 在行开头使用的‘/’进行注释的行为
ASFLAGS += -Wa,--divide -g
CXXFLAGS ?= -O0 -g
# 没有运行时支持，所以-fno-rtti、-fno-exceptions
CXXFLAGS += -std=gnu++14 -ffreestanding -fno-exceptions -fno-rtti -Wall -Wextra
CXXFLAGS += -I $(TO_ROOT)/kernel/include
LDFLAGS ?= -T $(TO_ROOT)/kernel/linker.ld -ffreestanding -nostdlib
LIBS ?= -lgcc

MKRESCUE ?= grub-mkrescue

ISO ?= INWOX.iso
