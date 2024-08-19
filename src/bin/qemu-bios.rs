use std::{
    env,
    process::{self, Command},
};

fn main() {
    let mut qemu = Command::new("qemu-system-x86_64");
    qemu.arg("-drive");
    qemu.arg(format!("format=raw,file={}", env!("BIOS_IMAGE")));
    qemu.arg("-serial");
    qemu.arg("stdio");
    qemu.arg("-m");
    qemu.arg("6G");
    let exit_status = qemu.status().unwrap();
    process::exit(exit_status.code().unwrap_or(-1));
}
