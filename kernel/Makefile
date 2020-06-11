TO_ROOT = ..
include $(TO_ROOT)/build-config/config.mk

BUILD = $(BUILD_DIR)/$(ARCH)/kernel

OBJ := kernel.o \
	print.o
OBJ += arch/i686/loader.o

OBJ := $(addprefix $(BUILD)/, $(OBJ))


all: $(BUILD)/kernel

$(BUILD)/kernel: $(OBJ)
	$(CC) $(LDFLAGS) -o $(BUILD)/kernel.elf $^ $(LIBS)

$(BUILD)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CXXFLAGS) -c -o $@ $^

$(BUILD)/%.o: src/%.s
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) -c -o $@ $^

clean:
	rm -rf $(BUILD)

.PHONY: all clean
