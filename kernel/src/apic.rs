use x2apic::ioapic::{IoApic, IrqMode, RedirectionTableEntry};
use x2apic::lapic::{LocalApic, LocalApicBuilder};
use x86_64::instructions::port::Port;
use x86_64::PhysAddr;

static mut LAPIC: LApic = LApic { lapic: None };

pub struct LApic {
    lapic: Option<LocalApic>,
}

impl LApic {
    pub fn enable() {
        unsafe { LAPIC.lapic.as_mut().unwrap().enable() }
    }

    pub fn id() -> u32 {
        unsafe { LAPIC.lapic.as_mut().unwrap().id() }
    }

    pub fn end_of_interrupt() {
        unsafe { LAPIC.lapic.as_mut().unwrap().end_of_interrupt() }
    }
}

pub fn init_local_apic(local_apic_address: PhysAddr) {
    disable_pic();

    let apic_virt_addr = crate::memory::phys_to_virt(local_apic_address);

    let local_apic = LocalApicBuilder::new()
        .timer_vector(32)
        .error_vector(51)
        .spurious_vector(0xff)
        .set_xapic_base(apic_virt_addr.as_u64())
        .build()
        .ok();
    unsafe { LAPIC.lapic = local_apic };
}

pub fn init_io_apic(io_apic_address: PhysAddr) {
    let apic_virt_addr = crate::memory::phys_to_virt(io_apic_address);
    unsafe {
        let mut ioapic = IoApic::new(apic_virt_addr.as_u64());

        ioapic.init(32);

        let mut entry = RedirectionTableEntry::default();
        entry.set_mode(IrqMode::Fixed);
        entry.set_vector(33);
        entry.set_dest(LApic::id() as u8);

        ioapic.set_table_entry(1, entry);

        ioapic.enable_irq(1);
    }
}

fn disable_pic() {
    let mut data_8259a = Port::<u8>::new(0x21);
    let mut data_8259b = Port::<u8>::new(0xa1);
    unsafe {
        data_8259a.write(u8::MAX);
        data_8259b.write(u8::MAX);
    }
}
