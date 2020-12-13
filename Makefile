TO_ROOT = .
include $(TO_ROOT)/build-config/config.mk

all: build_info install-headers libc install-libc kernel programs tests strip-debug iso
	@echo -n ${COLOR_RESET}

build_info:
	@echo ${COLOR_YELLOW}Build beginning...

install-headers:
	$(MAKE) -C libc install-headers

libc:
	$(MAKE) -C libc

install-libc:
	$(MAKE) -C libc install

kernel:
	$(MAKE) -C kernel

iso: $(ISO)

# 为调试生成kernel.sym，并删除kernel.elf中的多余调试信息
strip-debug:
	@echo ${COLOR_YELLOW}Building package...
	i686-inwox-objcopy --only-keep-debug $(BUILD_DIR)/$(ARCH)/kernel/kernel.elf $(BUILD_DIR)/kernel.sym
	i686-inwox-objcopy --strip-debug $(BUILD_DIR)/$(ARCH)/kernel/kernel.elf

$(ISO): $(BUILD_DIR)/$(ARCH)/kernel/kernel.elf $(INITRD)
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp $(BUILD_DIR)/$(ARCH)/kernel/kernel.elf iso/boot/kernel.elf
	cp $(INITRD)                              iso/
	cp $(BUILD_DIR)/tests/printf              iso/
	echo 'set timeout=0'                   >  iso/boot/grub/grub.cfg
	echo 'set default=0'                   >> iso/boot/grub/grub.cfg
	echo ''                                >> iso/boot/grub/grub.cfg
	echo 'menuentry "INWOX" {'             >> iso/boot/grub/grub.cfg
	echo '    multiboot /boot/kernel.elf'  >> iso/boot/grub/grub.cfg
	echo '    module /initrd.tar'          >> iso/boot/grub/grub.cfg
	echo '    module /printf'              >> iso/boot/grub/grub.cfg
	echo '    boot'                        >> iso/boot/grub/grub.cfg
	echo '}'                               >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=$(ISO) iso
	rm -rf iso

$(INITRD): $(SYSROOT)
	echo "INWOX 0.0.1-dev" > $(BIN_DIR)/inwox
	cd $(SYSROOT) && tar cvf ../$(INITRD) --format=ustar *

programs:
	$(MAKE) -C programs

tests:
	$(MAKE) -C tests

# How To Exit
# Windows Terminal: Alt + 2 -> q
# macOS Terminal: esc + 2 -> q
qemu-curses: $(ISO)
	qemu-system-i386 -cdrom $^ -curses -m 8M

qemu-curses-dbg: $(ISO)
	qemu-system-i386 -cdrom $^ -S -s -curses -m 8M

install-toolchain: install-headers
	SYSROOT=$(SYSROOT) $(TO_ROOT)/build-config/install-toolchains.sh

install-toolchain-from-china: install-headers
	SYSROOT=$(SYSROOT) $(TO_ROOT)/build-config/install-toolchains.sh cn

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(ISO)
	rm -rf ./sysroot ./iso

.PHONY: all kernel iso qemu clean libc install-headers install-libc strip-debug programs tests
