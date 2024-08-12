#![feature(abi_x86_interrupt)]
#![no_std]
#![no_main]

use core::panic::PanicInfo;

mod framebuffer;
mod interrupt;
mod serial;

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}

bootloader_api::entry_point!(kernel_main);
fn kernel_main(boot_info: &'static mut bootloader_api::BootInfo) -> ! {
    framebuffer::init_display(boot_info.framebuffer.as_mut().unwrap());
    interrupt::init_idt();
    unsafe {
        interrupt::PICS.lock().initialize();
    }
    x86_64::instructions::interrupts::enable();

    println!("Hi, This is INWOX OS{}", '!');
    serial_println!("Hi, This is INWOX OS{}", '!');
    loop {}
}
