#![feature(abi_x86_interrupt)]
#![no_std]
#![no_main]
extern crate alloc;

use crate::memory::BootInfoFrameAllocator;
use bootloader_api::config::Mapping;
use bootloader_api::BootloaderConfig;
use core::panic::PanicInfo;
use x86_64::VirtAddr;

mod framebuffer;
mod interrupt;
mod serial;
mod memory;
mod allocator;
mod acpi;
mod apic;

#[panic_handler]
fn panic(info: &PanicInfo) -> ! {
    serial_println!("{}", info);
    println!("{}", info);
    hlt_loop();
}

pub static BOOTLOADER_CONFIG: BootloaderConfig = {
    let mut config = BootloaderConfig::new_default();
    config.mappings.physical_memory = Some(Mapping::Dynamic);
    config
};

bootloader_api::entry_point!(kernel_main,config = &BOOTLOADER_CONFIG);
fn kernel_main(boot_info: &'static mut bootloader_api::BootInfo) -> ! {
    framebuffer::init_display(boot_info.framebuffer.as_mut().unwrap());
    interrupt::init_idt();

    let physical_memory_offset = VirtAddr::new(boot_info.physical_memory_offset.into_option()
        .expect("map-physical-memory config option must be enabled"));
    let mut mapper = unsafe { memory::init(physical_memory_offset) };
    let mut frame_allocator = unsafe { BootInfoFrameAllocator::init(&boot_info.memory_regions) };
    allocator::init_heap(&mut mapper, &mut frame_allocator).expect("heap initialization failed");

    acpi::init(boot_info.rsdp_addr.into_option().expect("init acpi failed"));


    x86_64::instructions::interrupts::enable();
    println!("Hi, This is INWOX OS{}", '!');
    hlt_loop();
}

pub fn hlt_loop() -> ! {
    loop {
        x86_64::instructions::hlt();
    }
}
