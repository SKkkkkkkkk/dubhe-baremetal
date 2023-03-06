#include <errno.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <assert.h>

#define CONSOLE_BAUDRATE 115200U
#define UART_CLK		 20000000U

#if  defined(M3)
	#define UART_ID SEEHI_UART0
#elif defined(A55)
	#define UART_ID SEEHI_UART1
#else
	#error "Unsupport Core!"
#endif

#ifndef __unused
	#define __unused __unused
#endif

#if defined QEMU
	#include "pl011.h"
	static bool uart_init = false;
	int _write (int fd __unused, char *ptr, int len)
	{
		if(!uart_init)
		{
			uart_config _uart_config = {
				.data_bits = 8,
				.stop_bits = 1,
				.parity = false,
				.baudrate = 9600
			};
			if(uart_configure(&_uart_config) != UART_OK)
				return 0;
			uart_init = true;
		}

		int i;
		for(i=0;i<len;i++)
		{
			uart_putchar(ptr[i]);
			if(ptr[i] == '\n')
				uart_putchar('\r');
		}
		return i;
	}

	int _read(int fd __unused, char* ptr, int len)
	{
		if(!uart_init)
		{
			uart_config _uart_config = {
				.data_bits = 8,
				.stop_bits = 1,
				.parity = false,
				.baudrate = 9600
			};
			if(uart_configure(&_uart_config) != UART_OK)
				return 0;
			uart_init = true;
		}


		int i;
		for(i=0;i<len;i++)
			while(uart_getchar(ptr+i)!=UART_OK);
		return i;
	}
#else
	#include "dw_apb_uart.h"
	static bool uart_init = false;
	int _write (int fd __unused, char *ptr, int len)
	{
		if(!uart_init)
		{
			if(seehi_uart_config_baudrate(CONSOLE_BAUDRATE, UART_CLK, UART_ID)!=0)
			{
				uart_init = false;
				return 0;
			}
			uart_init = true;
		}
		int i;
		for(i=0;i<len;i++)
		{
			uart_sendchar(UART_ID ,ptr[i]);
			if(ptr[i] == '\n')
				uart_sendchar(UART_ID,'\r');
		}
		return i;
	}

	int _read(int fd __unused, char* ptr, int len)
	{
		if(!uart_init)
		{
			if(seehi_uart_config_baudrate(CONSOLE_BAUDRATE, UART_CLK, UART_ID)!=0)
			{
				uart_init = false;
				return 0;
			}
			uart_init = true;
		}
		int i;
		for(i=0;i<len;i++)
		{
			ptr[i] = uart_getchar(UART_ID);
		}
		return i;
	}
#endif



/* _exit */
void _exit(int status __unused) {
	while(1);
}

/* close */
int _close(int file __unused) {
	return -1;
}

/* fstat */
int _fstat(int file __unused, struct stat *st) {
	st->st_mode = S_IFCHR;
	return 0;
}

int _getpid(void) {
	return 1;
}

int _isatty(int file __unused) {
	return 1;
}

int _kill(int pid __unused, int sig __unused) {
	errno = EINVAL;
	return -1;
}

int _lseek(int file __unused, int ptr __unused, int dir __unused) {
	return 0;
}

void *_sbrk(int incr) {
	extern char __HEAP_START__;
	extern char __HEAP_END__;
	static unsigned char *heap = (unsigned char *)(uintptr_t)(&__HEAP_START__);
	unsigned char *prev_heap;
	prev_heap = heap;
	if((uintptr_t)(heap + incr) > (uintptr_t)&__HEAP_END__)
	{
		_write(1, "Heap Overflow!\n\r", 16);
		while(1);
	}
	heap += incr;
	if((uintptr_t)heap&7)
		_write(1, "Heap is not Aligned!\n\r", 22);
	return prev_heap;
}

#if defined(FREERTOS)

#include "FreeRTOS.h"
#include "task.h"
void __malloc_lock(struct _reent *r __unused)   
{
	configASSERT(!xPortIsInsideInterrupt()); // 禁止在中断中使用malloc
	if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)
		return;
	vTaskSuspendAll();
}

void __malloc_unlock(struct _reent *r __unused) 
{
	configASSERT(!xPortIsInsideInterrupt()); // 禁止在中断中使用malloc
	if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)
		return;
	(void)xTaskResumeAll();
}
#endif

/* environment */
char *__env[1] = { 0 };
char **environ = __env;

int link(char *old __unused, char *new __unused) {
	errno = EMLINK;
	return -1;
}

int _open(const char *name __unused, int flags __unused, int mode __unused) {
	return -1;
}

/* execve */
// int execve(char *name __unused, char **argv __unused, char **env __unused) {
// 	errno = ENOMEM;
// 	return -1;
// }

/* fork */
int fork(void) {
	errno = EAGAIN;
	return -1;
}


int stat (const char *__restrict __path __unused, struct stat *__restrict __sbuf ) {
	__sbuf->st_mode = S_IFCHR;
	return 0;
}

// int times(struct tms *buf) {
//   return -1;
// }

int unlink(char *name __unused) {
	errno = ENOENT;
	return -1;
}

int wait(int *status __unused) {
	errno = ECHILD;
	return -1;
}

typedef void (*ptr_func_t)();
extern char __preinit_array_start;
extern char __preinit_array_end;

extern char __init_array_start;
extern char __init_array_end;

extern char __fini_array_start;
extern char __fini_array_end;

/** Call constructors for static objects
 */
void call_init_array() {
    uintptr_t* func = (uintptr_t*)&__preinit_array_start;
    while (func < (uintptr_t*)&__preinit_array_end) {
		(*(ptr_func_t)(*func))();
        func++;
    }

    func = (uintptr_t*)&__init_array_start;
    while (func < (uintptr_t*)&__init_array_end) {
        (*(ptr_func_t)(*func))();
        func++;
    }
}

/** Call destructors for static objects
 */
void call_fini_array() {
    ptr_func_t array = (ptr_func_t)&__fini_array_start;
    while (array < (ptr_func_t)&__fini_array_end) {
        (*array)();
        array++;
    }
}



void _fini()
{
	return;
}


#if defined(A55) && !defined(aarch64_libc)

#ifdef A55
#include "arch_features.h"
#endif

void *__wrap_memcpy(void *dst, const void *src, size_t len)
{
	if( ((read_sctlr_el3() & SCTLR_M_BIT) == SCTLR_M_BIT) && ((read_sctlr_el3() & SCTLR_A_BIT) == 0ULL) )
	{
		// _write(1, "__real_memcpy\n\r", 15);
		extern void *__real_memcpy(void *dst, const void *src, size_t len);
		return __real_memcpy(dst, src, len);
	}

	// _write(1, "__aligned_memcpy\n\r", 18);
	const char *s = src;
	char *d = dst;

	while (len--)
		*d++ = *s++;

	return dst;
}

void *__wrap_memmove(void *dst, const void *src, size_t len)
{
	if( ((read_sctlr_el3() & SCTLR_M_BIT) == SCTLR_M_BIT) && ((read_sctlr_el3() & SCTLR_A_BIT) == 0ULL) )
	{
		// _write(1, "__real_memmove\n\r", 16);
		extern void *__real_memmove(void *dst, const void *src, size_t len);
		return __real_memmove(dst, src, len);
	}
	// _write(1, "__aligned_memmove\n\r", 16);
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

void *__wrap_memset(void *dst, int val, size_t count)
{
	if( ((read_sctlr_el3() & SCTLR_M_BIT) == SCTLR_M_BIT) && ((read_sctlr_el3() & SCTLR_A_BIT) == 0ULL) )
	{
		// _write(1, "__real_memset\n\r", 15);
		extern void *__real_memset(void *dst, int val, size_t count);
		return __real_memset(dst, val, count);
	}

	// _write(1, "__aligned_memset\n\r", 18);
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
#endif