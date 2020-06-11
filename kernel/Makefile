TO_ROOT = ..
include $(TO_ROOT)/build-config/config.mk

BUILD = $(BUILD_DIR)/$(ARCH)/kernel

OBJ := \
	addressspace.o \
	gdt.o \
	idt.o \
	interrupt.o \
	kernel.o \
	pic.o \
	physicalmemory.o \
	print.o \
	process.o

OBJ += arch/i686/interrupt.o \
	arch/i686/loader.o

CRTBEGIN_0 := $(shell $(CC) $(CXXFLAGS) -print-file-name=crtbegin.o)
CRTEND_0 := $(shell $(CC) $(CXXFLAGS) -print-file-name=crtend.o)

START_OBJ := $(BUILD)/arch/i686/crti.o $(CRTBEGIN_0)
END_OBJ := $(CRTEND_0) $(BUILD)/arch/i686/crtn.o

OBJ := $(START_OBJ) $(addprefix $(BUILD)/, $(OBJ)) $(END_OBJ)

all: $(BUILD)/kernel

$(BUILD)/kernel: $(OBJ)
	$(CXX) $(LDFLAGS) -o $(BUILD)/kernel.elf $^ $(LIBS)

$(BUILD)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $^

$(BUILD)/%.o: src/%.s
	@mkdir -p $(dir $@)
	$(CXX) $(ASFLAGS) $(CPPFLAGS) -c -o $@ $^

clean:
	rm -rf $(BUILD)

.PHONY: all clean
