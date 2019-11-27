#include <io.h>
#include <output.h>
#include <setup.h>

#define PCI_CONFIG_ADDRESS	0xcf8
#define  PCI_CONFIG_A_ENABLE			0x80000000
#define  PCI_CONFIG_A_ADDR(bus, devfn, reg)	((bus) << 16 | (devfn) << 8 | (reg) << 2)
#define  PCI_CONFIG_REG_IDS	0x0
#define PCI_CONFIG_DATA		0xcfc

static void pci_print_id(unsigned int id)
{
	print_num((id >> 16) & 0xff);
	print(":");
	print_num((id >> 11) & 0x1f);
	print(".");
	print_num((id >> 8)  & 0x7);
	//print_num((id >> 2)  & 0x3f);
}

void main(void)
{
	unsigned int reg;
	unsigned int bus, devfn;

	clear_screen();

	print_color("PCI enumeration example\n", 0x09);

	for (bus = 0; bus < 256; bus++)
		for (devfn = 0; devfn < 256; devfn++) {
			unsigned int id = PCI_CONFIG_A_ADDR(bus, devfn, PCI_CONFIG_REG_IDS);

			outl(PCI_CONFIG_A_ENABLE | id, PCI_CONFIG_ADDRESS);

			reg = inl(PCI_CONFIG_DATA);
			if (reg == ~0)
				continue;

			pci_print_id(id);
			print("   ");
			print_num(reg & 0xffff);
			print_num((reg >> 16) & 0xffff);
			print("\n");
		}

	outl(0, PCI_CONFIG_ADDRESS);

	halt();
}
