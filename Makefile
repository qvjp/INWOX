TO_ROOT = .
include $(TO_ROOT)/build-config/config.mk

all: build-info install-headers libc install-libc kernel programs tests gen-debug-sym iso
	@echo -n ${COLOR_RESET}

build-info:
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
gen-debug-sym:
	$(ARCH)-inwox-objcopy --only-keep-debug $(BUILD_DIR)/$(ARCH)/kernel/kernel.elf $(BUILD_DIR)/kernel.sym

strip-debug:
	$(ARCH)-inwox-objcopy --strip-debug $(BUILD_DIR)/$(ARCH)/kernel/kernel.elf

$(ISO): $(BUILD_DIR)/$(ARCH)/kernel/kernel.elf $(INITRD)
	@echo ${COLOR_YELLOW}Packing...
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
	cd $(SYSROOT) && tar cvf ../$(INITRD) --format=ustar *

programs:
	$(MAKE) -C programs

tests:
	$(MAKE) -C tests

qemu: $(ISO)
	qemu-system-i386 -cdrom $^ -m 8M

# How To Exit
# Windows Terminal: Alt + 2 -> q
# macOS Terminal: esc + 2 -> q
qemu-curses: $(ISO)
	qemu-system-i386 -cdrom $^ -display curses -m 8M

qemu-curses-dbg: $(ISO)
	qemu-system-i386 -cdrom $^ -S -s -display curses -m 8M

install-toolchain: install-headers
	SYSROOT=$(SYSROOT) $(TO_ROOT)/build-config/install-toolchains.sh

install-toolchain-from-china: install-headers
	SYSROOT=$(SYSROOT) $(TO_ROOT)/build-config/install-toolchains.sh cn

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(ISO)
	rm -rf ./sysroot ./iso

.PHONY: all kernel iso qemu-curses qemu-curses-dbg clean libc install-headers install-libc gen-debug-sym strip-debug programs tests
