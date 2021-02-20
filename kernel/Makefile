TO_ROOT = ..
include $(TO_ROOT)/build-config/config.mk
include $(TO_ROOT)/build-config/version.mk
CPPFLAGS += -D__is_inwox_kernel
CPPFLAGS += -DINWOX_VERSION=\"$(INWOX_VERSION)\"

BUILD = $(BUILD_DIR)/$(ARCH)/kernel

OBJ = \
	addressspace.o \
	assert.o \
	directory.o \
	file.o \
	filedescription.o \
	gdt.o \
	idt.o \
	initrd.o \
	interrupt.o \
	kernel.o \
	keyboard.o \
	kthread.o \
	memorysegment.o \
	pic.o \
	pit.o \
	physicalmemory.o \
	ps2.o \
	ps2keyboard.o \
	print.o \
	process.o \
	syscall.o \
	terminal.o \
	timer.o \
	uname.o \
	vgaterminal.o \
	vnode.o

OBJ += arch/i686/interrupt.o \
	arch/i686/loader.o \
	arch/i686/syscall.o

CRTI_O = $(shell $(CXX) $(CXXFLAGS) -print-file-name=crti.o)
CRTBEGIN_O = $(shell $(CC) $(CXXFLAGS) -print-file-name=crtbegin.o)
CRTEND_O = $(shell $(CC) $(CXXFLAGS) -print-file-name=crtend.o)
CRTN_O = $(shell $(CXX) $(CXXFLAGS) -print-file-name=crtn.o)

START_OBJ = $(CRTI_O) $(CRTBEGIN_O)
END_OBJ = $(CRTEND_O) $(CRTN_O)

OBJ := $(START_OBJ) $(addprefix $(BUILD)/, $(OBJ)) $(END_OBJ)

all: build-info $(BUILD)/kernel
	@echo -n ${COLOR_RESET}

build-info:
	@echo ${COLOR_BLUE}Building kernel...

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
