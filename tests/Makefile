ARCH ?= i686

TO_ROOT = ..
include $(TO_ROOT)/build-config/config.mk

BUILD = $(BUILD_DIR)/tests

CFLAGS ?= -O2 -g

PROGRAMS = \
	printf

all: $(addprefix $(BUILD)/, $(PROGRAMS))

$(BUILD)/%: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^

clean:
	rm -rf $(BUILD)

.PHONY: all clean
