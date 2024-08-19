use crate::apic;
use acpi::{AcpiHandler, AcpiTables, InterruptModel, PhysicalMapping};
use core::ptr::NonNull;
use x86_64::PhysAddr;

pub fn init(rsdp_addr: u64) {
    let tables = unsafe { AcpiTables::from_rsdp(Handler, rsdp_addr as usize).expect("read acpi tables from rsdp failed") };
    let platform_info = tables.platform_info().unwrap();

    if let InterruptModel::Apic(apic) = platform_info.interrupt_model {
        apic::init_local_apic(PhysAddr::new(apic.local_apic_address));
        apic::init_io_apic(PhysAddr::new(apic.io_apics.first().unwrap().address as u64));
        apic::LApic::enable();
    }
}

#[derive(Clone, Debug, Copy)]
pub struct Handler;

impl AcpiHandler for Handler {
    unsafe fn map_physical_region<T>(&self, physical_address: usize, size: usize) -> PhysicalMapping<Self, T> {
        let phys_addr = PhysAddr::new(physical_address as u64);
        let virt_addr = crate::memory::phys_to_virt(phys_addr);
        let ptr = NonNull::new(virt_addr.as_mut_ptr()).unwrap();
        PhysicalMapping::new(physical_address, ptr, size, size, Self)
    }

    fn unmap_physical_region<T>(_region: &PhysicalMapping<Self, T>) {}
}
