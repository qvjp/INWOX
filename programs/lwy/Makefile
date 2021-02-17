TO_ROOT = ../..
include $(TO_ROOT)/build-config/programs.mk

EXEC_NAME=lwy

PROG_DIR = $(BUILD_DIR)/$(EXEC_NAME)
BUILD = $(PROG_DIR)/$(EXEC_NAME)
TARGET_SOURCE = src/lwy.c

all: $(BUILD)

$(TARGET_SOURCE): src/lwy.txt
	awk '1;NR==2{system("xxd -i src/lwy.txt")}' src/lwy.stub > $(TARGET_SOURCE)

$(BUILD): $(TARGET_SOURCE)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^
	rm -rf $(TARGET_SOURCE)

.PHONY: all $(BUILD) $(TARGET_SOURCE)
