ARCH ?= i686

TO_ROOT = ..
include $(TO_ROOT)/build-config/config.mk

BUILD = $(BUILD_DIR)/tools

CFLAGS ?= -O0 -g
CFLAGS += -Wall -Wextra

PROGRAMS = \
	bar \
	foo

all: build_info install-tools
	@echo -n ${COLOR_RESET}

build_info:
	@echo ${COLOR_CYAN}Building tools...

install-tools: $(addprefix $(BUILD)/, $(PROGRAMS))
	@mkdir -p $(BIN_DIR)
	cp -f $^ $(BIN_DIR)

$(BUILD)/%: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^

clean:
	rm -rf $(BUILD)

.PHONY: all install-tools clean
