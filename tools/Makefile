ARCH ?= i686

TO_ROOT = ..
include $(TO_ROOT)/build-config/config.mk

BUILD = $(BUILD_DIR)/tools

CFLAGS ?= -O0 -g
CFLAGS += -Wall -Wextra

PROGRAMS = \
	bar \
	foo

install-tools: $(addprefix $(BUILD)/, $(PROGRAMS))
	@mkdir -p $(BIN_DIR)
	cp -f $^ $(BIN_DIR)

all: $(addprefix $(BUILD)/, $(PROGRAMS))

$(BUILD)/%: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^

clean:
	rm -rf $(BUILD)

.PHONY: all install-tools clean
