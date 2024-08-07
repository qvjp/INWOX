#![no_std]
#![no_main]

use core::panic::PanicInfo;

mod framebuffer;

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}

bootloader_api::entry_point!(kernel_main);
fn kernel_main(boot_info: &'static mut bootloader_api::BootInfo) -> ! {
    framebuffer::init_display(boot_info.framebuffer.as_mut().unwrap());

    println!("Hi, This is INWOX OS{}", '!');
    loop {}
}
