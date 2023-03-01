#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "irq_ctrl.h"
#ifndef QEMU
	#include "system_counter.h"
#endif

void *memcpy(void *dst, const void *src, size_t len)
{
	const char *s = src;
	char *d = dst;

	while (len--)
		*d++ = *s++;

	return dst;
}

void *memmove(void *dst, const void *src, size_t len)
{
	/*
	 * The following test makes use of unsigned arithmetic overflow to
	 * more efficiently test the condition !(src <= dst && dst < str+len).
	 * It also avoids the situation where the more explicit test would give
	 * incorrect results were the calculation str+len to overflow (though
	 * that issue is probably moot as such usage is probably undefined
	 * behaviour and a bug anyway.
	 */
	if ((size_t)dst - (size_t)src >= len) {
		/* destination not in source data, so can safely use memcpy */
		return memcpy(dst, src, len);
	} else {
		/* copy backwards... */
		const char *end = dst;
		const char *s = (const char *)src + len;
		char *d = (char *)dst + len;
		while (d != end)
			*--d = *--s;
	}
	return dst;
}

void *memset(void *dst, int val, size_t count)
{
	uint8_t *ptr = dst;
	uint64_t *ptr64;
	uint64_t fill = (unsigned char)val;

	/* Simplify code below by making sure we write at least one byte. */
	if (count == 0U) {
		return dst;
	}

	/* Handle the first part, until the pointer becomes 64-bit aligned. */
	while (((uintptr_t)ptr & 7U) != 0U) {
		*ptr = (uint8_t)val;
		ptr++;
		if (--count == 0U) {
			return dst;
		}
	}

	/* Duplicate the fill byte to the rest of the 64-bit word. */
	fill |= fill << 8;
	fill |= fill << 16;
	fill |= fill << 32;

	/* Use 64-bit writes for as long as possible. */
	ptr64 = (uint64_t *)ptr;
	for (; count >= 8U; count -= 8) {
		*ptr64 = fill;
		ptr64++;
	}

	/* Handle the remaining part byte-per-byte. */
	ptr = (uint8_t *)ptr64;
	while (count-- > 0U)  {
		*ptr = (uint8_t)val;
		ptr++;
	}

	return dst;
}



int main()
{
	printf("hello world.\n\r");
#ifndef QEMU
	initSystemCounter(0, 0);
#endif
	IRQ_Initialize();
	void task1(void* arg);
	void task2(void* arg);
	if (xTaskCreate(task1, "task1", configMINIMAL_STACK_SIZE*2, NULL, 1, NULL) != pdPASS)
		while (1)
			;
	if (xTaskCreate(task2, "task2", configMINIMAL_STACK_SIZE*2, NULL, 1, NULL) != pdPASS)
		while (1)
			;
	vTaskStartScheduler();
	return 0;
}