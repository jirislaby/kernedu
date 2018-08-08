#include <string.h>

void memcpy(void *dst, const void *src, const unsigned count)
{
	unsigned a;
	for (a = 0; a < count; a++)
		*(char *)dst++ = *(char *)src++;
}
