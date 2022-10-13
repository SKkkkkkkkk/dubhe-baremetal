#include <stdint.h>
#include <string.h>

char *
strcpy(char *restrict s1, const char *restrict s2)
{
	char *ret = s1;

	while ((*s1++ = *s2++) != '\0')
		;

	return ret;
}

/*
 * Copy src to dst, truncating or null-padding to always copy n bytes.
 * Return dst.
 */
char *
strncpy(char * __restrict dst, const char * __restrict src, size_t n)
{
	if (n != 0) {
		char *d = dst;
		const char *s = src;

		do {
			if ((*d++ = *s++) == '\0') {
				/* NUL pad the remaining n-1 bytes */
				while (--n != 0)
					*d++ = '\0';
				break;
			}
		} while (--n != 0);
	}
	return (dst);
}