ARCH ?= i686

TO_ROOT = ..
include $(TO_ROOT)/build-config/config.mk

BUILD = $(BUILD_DIR)/tools

LDFLAGS := --sysroot=$(SYSROOT) -nostdlib

PROGRAMS = \
	bar \
	foo

all: $(addprefix $(BUILD)/, $(PROGRAMS))

$(BUILD)/%: $(BUILD)/%.o
	$(CC) $(LDFLAGS) -o $@ $^

$(BUILD)/%.o: %.s
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) -c -o $@ $^

clean:
	rm -rf $(BUILD)

.PHONY: all clean
