#include "../x86.h"
#include "pic.h"

void PIC_sendEOI(uint8_t irq) {
        if(irq >= 8) {
                outb(PIC2_COMMAND, PIC_EOI);                                                                                    }

        outb(PIC1_COMMAND, PIC_EOI);
}

void PIC_remap(uint8_t offset1) {
	uint8_t master_mask, slave_mask;

	master_mask = inb(PIC1_DATA);
	slave_mask = inb(PIC2_DATA);
	
	// This will send ICW1 saying we'll follow with ICW4 later on
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);	// starts initialization
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	
	// Send ICW2 with IRQ remapping
	outb(PIC1_DATA, offset1);
	io_wait();
	outb(PIC2_DATA, offset1 + 0x08);	// This is supposed to be offset1+8, so I just have to remember that.
	io_wait();
	
	outb(PIC1_DATA, 0x04);
	io_wait();
	outb(PIC2_DATA, 0x02);
	io_wait();

	outb(PIC1_DATA, ICW4_8086);
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();

	// And we're done
	outb(PIC1_DATA, master_mask);
	outb(PIC2_DATA, slave_mask);
}

//  Disables interrupts. Really only had this for testing, don't think I'll ever use it? After today, I've decided to once again have tomorrow
//  be a study day, because this is all a tad bit confusing, and I would like to understand it before moving onto the next step, which is 
//  actually implementing various ISR's. 
void pic_disable() {
	outb(PIC1_DATA, 0xFF);
	outb(PIC2_DATA, 0xFF);
}

void IRQ_set_mask(uint8_t IRQline) {
        uint16_t port;
        uint8_t value;

        if(IRQline < 8) {
                port = PIC1_DATA;
        }
        else {
                port = PIC2_DATA;
                IRQline -= 8;
        }
        value = inb(port) | (1 << IRQline);
        outb(port, value);
}

void IRQ_clear_mask(uint8_t IRQline) {
        uint16_t port;
        uint8_t value;

        if(IRQline < 8) {
                port = PIC1_DATA;
        }
        else {
                port = PIC2_DATA;
                IRQline -= 8;
        }
        value = inb(port) & ~(1 << IRQline);
        outb(port, value);
}

#define PIC_READ_IRR    0x0A
#define PIC_READ_ISR    0x0B

static uint16_t __pic_get_irq_reg(int ocw3) {
        outb(PIC1_COMMAND, ocw3);
        outb(PIC2_COMMAND, ocw3);
        return (inb(PIC2_COMMAND) << 8) | inb(PIC1_COMMAND);
}

uint16_t pic_get_irr(void) {
        return __pic_get_irq_reg(PIC_READ_IRR);
}

uint16_t pic_get_isr(void) {
        return __pic_get_irq_reg(PIC_READ_ISR);
}
