ARCH ?= i686
BUILD_DIR = $(TO_ROOT)/build/programs
SYSROOT ?= $(TO_ROOT)/sysroot
BIN_DIR = $(SYSROOT)/bin

AR := $(ARCH)-inwox-ar
CC := $(ARCH)-inwox-gcc
CXX := $(ARCH)-inwox-g++

CFLAGS ?= -O0 -g
CFLAGS += -Wall -Wextra
CPPFLAGS += -I $(TO_ROOT)/kernel/include -I $(SYSROOT)/usr/include

COLOR_BLUE="\033[1;34m"
COLOR_GREEN="\033[1;32m"
COLOR_YELLOW="\033[1;33m"
COLOR_CYAN="\033[1;36m"
COLOR_RESET="\033[0m"
