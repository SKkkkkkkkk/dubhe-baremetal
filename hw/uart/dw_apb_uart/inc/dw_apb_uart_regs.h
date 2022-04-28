#ifndef __APE1210_UART_REGS_H__
#define __APE1210_UART_REGS_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __IOM
#define     __IOM    volatile
#endif
#ifndef RESERVED
#define RESERVED(N, T) T RESERVED##N;
#endif
typedef struct {
	union { /* 0x00 */
		__IOM uint32_t RBR;
		__IOM uint32_t THR;
		__IOM uint32_t DLL;
	};
	union { /* 0x04 */
		__IOM uint32_t DLH; /* 0x04 */
		__IOM uint32_t IER; /* 0x04 */
	};
	union {
		__IOM uint32_t IIR; /* 0x08 */
		__IOM uint32_t FCR; /* 0x08 */
	};
	__IOM uint32_t LCR; /* 0x0c */
	__IOM uint32_t MCR; /* 0x10 */
	__IOM uint32_t LSR; /* 0x14 */
	__IOM uint32_t MSR; /* 0x18 */
	__IOM uint32_t SCR; /* 0x1c */
	__IOM uint32_t LPDLL; /* 0x20 */
	__IOM uint32_t LPDLH; /* 0x24 */
	RESERVED(0 [2], uint32_t) /* 0x28 ~ 0x2c */
	union {
		__IOM uint32_t SRBR[16]; /* 0x30 ~ 0x6c */
		__IOM uint32_t STHR[16]; /* 0x30 ~ 0x6c */
	};
	__IOM uint32_t FAR; /* 0x70 */
	__IOM uint32_t TFR; /* 0x74 */
	__IOM uint32_t RFW; /* 0x78 */
	__IOM uint32_t USR; /* 0x7c */
	__IOM uint32_t TFL; /* 0x80 */
	__IOM uint32_t RFL; /* 0x84 */
	__IOM uint32_t SRR; /* 0x88 */
	__IOM uint32_t SRTS; /* 0x8c */
	__IOM uint32_t SBCR; /* 0x90 */
	__IOM uint32_t SDMAM; /* 0x94 */
	__IOM uint32_t SFE; /* 0x98 */
	__IOM uint32_t SRT; /* 0x9c */
	__IOM uint32_t STET; /* 0xa0 */
	__IOM uint32_t HTX; /* 0xa4 */
	__IOM uint32_t DMASA; /* 0xa8 */
	__IOM uint32_t TCR; /* 0xac */
	__IOM uint32_t DE_EN; /* 0xb0 */
	__IOM uint32_t RE_EN; /* 0xb4 */
	__IOM uint32_t DET; /* 0xbc */
	__IOM uint32_t TAT; /* 0xb8 */
	__IOM uint32_t DLF; /* 0xc0 */
	__IOM uint32_t RAR; /* 0xc4 */
	__IOM uint32_t TAR; /* 0xc8 */
	__IOM uint32_t LCR_EXT; /* 0xcc */
	__IOM uint32_t UART_PROT_LEVEL; /* 0xd0 */
	__IOM uint32_t REG_TIMEOUT_RST; /* 0xd4 */
	RESERVED(1 [7], uint32_t) /* 0xd8 ~ f0 */
	__IOM uint32_t CPR; /* 0xf4 */
	__IOM uint32_t UCR; /* 0xf8 */
	__IOM uint32_t CTR; /* 0xfc */
} APE1210_UART_TypeDef;

#ifdef __cplusplus
}
#endif

#endif
