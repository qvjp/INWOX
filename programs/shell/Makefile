TO_ROOT = ../..
include $(TO_ROOT)/build-config/programs.mk

EXEC_NAME=shell

PROG_DIR = $(BUILD_DIR)/$(EXEC_NAME)
BUILD = $(PROG_DIR)/$(EXEC_NAME)

all: $(BUILD)

$(BUILD): src/shell.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^

.PHONY: all $(BUILD)
