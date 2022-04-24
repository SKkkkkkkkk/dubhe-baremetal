#if defined A55
#include <errno.h>
#include <sys/stat.h>
#if defined QEMU
	#include "pl001.h"
#endif

#undef errno
extern int errno;

#if defined QEMU
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
				.stop_bits = 0,
				.parity = true,
				.baudrate = 115200
			};
			if(uart_configure(&_uart_config) != UART_OK)
				return 0;
			uart_init = true;
		}


		int i;
		for(i=0;i<len;i++)
		{
			if(uart_getchar(ptr+i) != UART_OK)
				return 0;
		}
		(void)fd;
		return i;
	}
#else
	int _write (int fd, char *ptr, int len)
	{
		(void)fd;
		(void)ptr;
		(void)len;
		return 0;
	}

	int _read(int fd, char* ptr, int len)
	{
		(void)fd;
		(void)ptr;
		(void)len;
		return 0;
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
	// while(1);
	extern char __HEAP_START__;
	extern char __HEAP_END__;
	static unsigned char *heap = (unsigned char *)(uintptr_t)(&__HEAP_START__);
	unsigned char *prev_heap;

	// if (heap == NULL) {
		// uint32_t tmp = ((uint32_t)&__HEAP_START__)&7;
		// if(tmp)
		// 	heap = (unsigned char *)(8 - tmp + (uint32_t)(&__HEAP_START__));
		// else
			// heap = (unsigned char *)(uint32_t)(&__HEAP_START__);
	// }
	prev_heap = heap;

	// if(incr&7)
	// {
	// 	incr += (8 - (incr&7));
	// }

	if((uintptr_t)(heap += incr) > (uintptr_t)&__HEAP_END__)
	{
		while(1);
	}
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
#endif