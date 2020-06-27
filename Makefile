TO_ROOT = .
include $(TO_ROOT)/build-config/config.mk

all: install-headers libc install-libc kernel tools strip-debug iso

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
	i686-inwox-objcopy --only-keep-debug $(BUILD_DIR)/$(ARCH)/kernel/kernel.elf $(BUILD_DIR)/kernel.sym
	i686-inwox-objcopy --strip-debug $(BUILD_DIR)/$(ARCH)/kernel/kernel.elf

$(ISO): $(BUILD_DIR)/$(ARCH)/kernel/kernel.elf
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp $(BUILD_DIR)/$(ARCH)/kernel/kernel.elf iso/boot/kernel.elf
	cp $(BUILD_DIR)/tools/bar                 iso/
	cp $(BUILD_DIR)/tools/foo                 iso/
	echo 'set timeout=0'                   >  iso/boot/grub/grub.cfg
	echo 'set default=0'                   >> iso/boot/grub/grub.cfg
	echo ''                                >> iso/boot/grub/grub.cfg
	echo 'menuentry "INWOX" {'             >> iso/boot/grub/grub.cfg
	echo '    multiboot /boot/kernel.elf'  >> iso/boot/grub/grub.cfg
	echo '    module /bar'                 >> iso/boot/grub/grub.cfg
	echo '    module /foo'                 >> iso/boot/grub/grub.cfg
	echo '    boot'                        >> iso/boot/grub/grub.cfg
	echo '}'                               >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=$(ISO) iso
	rm -rf iso

tools:
	$(MAKE) -C tools

# How To Exit
# Windows Terminal: Alt + 2 -> q
# macOS Terminal: esc + 2 -> q
qemu-curses: $(ISO)
	qemu-system-i386 -cdrom $^ -curses -m 8M

qemu-curses-dbg: $(ISO)
	qemu-system-i386 -cdrom $^ -S -s -curses -m 8M

install-toolchain:
	$(TO_ROOT)/build-config/install-toolchains.sh

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(ISO)
	rm -rf ./sysroot ./iso

.PHONY: all kernel iso qemu clean libc install-headers install-libc strip-debug tools
