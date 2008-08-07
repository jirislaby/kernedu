#ifndef __OUTPUT_H_FILE
#define __OUTPUT_H_FILE

extern void clear_screen(void);
extern void print_color(const char *text, unsigned char color);
extern void print_num_color(unsigned long num, unsigned char color);

static inline void print(const char *text)
{
	print_color(text, 0x07);
}

static inline void print_num(unsigned long num)
{
	print_num_color(num, 0x07);
}


#endif
