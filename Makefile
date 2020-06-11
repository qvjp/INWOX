TO_ROOT = .
include $(TO_ROOT)/build-config/config.mk

all: kernel iso

kernel:
	$(MAKE) -C kernel

iso: $(ISO)

$(ISO): $(BUILD_DIR)/$(ARCH)/kernel/kernel.elf
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp $(BUILD_DIR)/$(ARCH)/kernel/kernel.elf iso/boot/kernel.elf
	echo 'set timeout=0'                   >  iso/boot/grub/grub.cfg
	echo 'set default=0'                   >> iso/boot/grub/grub.cfg
	echo ''                                >> iso/boot/grub/grub.cfg
	echo 'menuentry "INWOX" {'             >> iso/boot/grub/grub.cfg
	echo '    multiboot /boot/kernel.elf'  >> iso/boot/grub/grub.cfg
	echo '    boot'                        >> iso/boot/grub/grub.cfg
	echo '}'                               >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=$(ISO) iso
	rm -rf iso

qemu-curses: $(ISO)
	qemu-system-i386 -cdrom $^ -curses

install-toolchain:
	$(TO_ROOT)/build-config/install-toolchains.sh

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(ISO)

.PHONY: all kernel iso qemu clean
