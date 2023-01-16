#include <errno.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <assert.h>
#undef errno
extern int errno;

#ifdef A55
	#error "A55 has the aarch64_libc, don't use newlib."
#endif

#define CONSOLE_BAUDRATE 115200U
#define UART_CLK		 20000000U

#if defined QEMU
	#include "pl011.h"
	static bool uart_init = false;
	int _write (int fd, char *ptr, int len)
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
		(void)fd;
		return i;
	}

	int _read(int fd, char* ptr, int len)
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
		(void)fd;
		return i;
	}
#else
	#include "dw_apb_uart.h"
	static bool uart_init = false;
	int _write (int fd, char *ptr, int len)
	{
		if(!uart_init)
		{
			if(seehi_uart_config_baudrate(CONSOLE_BAUDRATE, UART_CLK, SEEHI_UART0)!=0)
			{
				uart_init = false;
				return 0;
			}
			uart_init = true;
		}
		int i;
		for(i=0;i<len;i++)
		{
			uart_sendchar(SEEHI_UART0 ,ptr[i]);
			if(ptr[i] == '\n')
				uart_sendchar(SEEHI_UART0,'\r');
		}
		(void)fd;
		return i;
	}

	int _read(int fd, char* ptr, int len)
	{
		if(!uart_init)
		{
			if(seehi_uart_config_baudrate(CONSOLE_BAUDRATE, UART_CLK, SEEHI_UART0)!=0)
			{
				uart_init = false;
				return 0;
			}
			uart_init = true;
		}
		int i;
		for(i=0;i<len;i++)
		{
			ptr[i] = uart_getchar(SEEHI_UART0);
		}
		(void)fd;
		return i;
	}
#endif



/* _exit */
void _exit(int status) {
	while(1);
}

/* close */
int _close(int file) {
	return -1;
}

/* fstat */
int _fstat(int file, struct stat *st) {
	st->st_mode = S_IFCHR;
	return 0;
}

int _getpid(void) {
	return 1;
}

int _isatty(int file) {
	return 1;
}

int _kill(int pid, int sig) {
	errno = EINVAL;
	return -1;
}

int _lseek(int file, int ptr, int dir) {
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
		assert(0);
		while(1);
	}
	heap += incr;
	return prev_heap;
}


/* environment */
char *__env[1] = { 0 };
char **environ = __env;

int link(char *old, char *new) {
	errno = EMLINK;
	return -1;
}

int open(const char *name, int flags, int mode) {
	return -1;
}

/* execve */
int execve(char *name, char **argv, char **env) {
	errno = ENOMEM;
	return -1;
}

/* fork */
int fork(void) {
	errno = EAGAIN;
	return -1;
}


int stat (const char *__restrict __path, struct stat *__restrict __sbuf ) {
	__sbuf->st_mode = S_IFCHR;
	return 0;
}

// int times(struct tms *buf) {
//   return -1;
// }

int unlink(char *name) {
	errno = ENOENT;
	return -1;
}

int wait(int *status) {
	errno = ECHILD;
	return -1;
}