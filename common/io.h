#ifndef __IO_H_FILE
#define __IO_H_FILE

#define pa(x)		(((unsigned long)x) + 0x7c00)
#define va(x)		((void *)((x) - 0x7c00))

#define barrier()	__asm__ __volatile__("": : :"memory")

static inline void outb(unsigned char val, unsigned short p)
{
	asm volatile("outb %0,%1" : : "a" (val), "dN" (p));
}
static inline unsigned char inb(unsigned short p)
{
	unsigned char val;
	asm volatile("inb %1,%0" : "=a" (val) : "dN" (p));
	return val;
}

static inline void outl(unsigned int val, unsigned short p)
{
	asm volatile("outl %0,%1" : : "a" (val), "dN" (p));
}
static inline unsigned int inl(unsigned short p)
{
	unsigned int val;
	asm volatile("inl %1,%0" : "=a" (val) : "dN" (p));
	return val;
}

static inline void sti(void)
{
	asm volatile("sti" : : : "memory");
}

static inline void cli(void)
{
	asm volatile("cli" : : : "memory");
}

static inline void hlt(void)
{
	asm volatile("hlt" : : : "memory");
}

#endif
