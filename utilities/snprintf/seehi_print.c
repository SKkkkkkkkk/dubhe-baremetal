#include "seehi_print.h"
#include "snprintf.h"

#define PRINT_APB_BASE        0xFF21B000

#ifndef FPGA
#define REG_BASE_SIMDBG  (PRINT_APB_BASE + 0x0)

#define SIM_PUTC (REG_BASE_SIMDBG  + 0x0)
#define SIM_PUTD (REG_BASE_SIMDBG  + 0x4)
#define SIM_PUTH (REG_BASE_SIMDBG  + 0x8)

#define writel(d, a) *(volatile unsigned int *)(a) = (d)
#define readl(a) (*(volatile unsigned int *)(a))


#define print_c(c) writel((c), SIM_PUTC)

#define print_str(s)                            \
do { \
	const char *ptmp = s; \
	while (*ptmp) { \
		writel(*ptmp, SIM_PUTC); \
		ptmp++; \
	} \
} while (0)	

#define print_int(i) \
do { \
  	writel(i, SIM_PUTD); \
} while (0)

#define print_hex(i) \
do { \
  	writel(i, SIM_PUTH); \
} while (0)

void simprintf(const char *fmt, ...)
{
    va_list ap;
    int d;
    char c, *s;

    va_start(ap, fmt);
    c = *fmt++;
    while (c)
    {
        if ('%' == c)
        {
            c = *fmt++;
            switch (c)
            {
            case 's': /* string */
                s = va_arg(ap, char *);
                print_str(s);
                break;
            case 'p': /* fixed-point */
                d = va_arg(ap, int);
                if (d < 0)
                {
                    d = -d;
                    print_c('-');
                }
                print_int(d / 1000);
                print_c('.');
                d = d % 1000;
                if (d < 100)
                    print_c('0');
                if (d < 10)
                    print_c('0');
                print_int(d);
                break;
            case 'd': /* int */
                d = va_arg(ap, int);
                print_int(d);
                break;
            case 'x': /* int */
                d = va_arg(ap, int);
                print_hex(d);
                break;
            case 'c': /* char */
                /* need a cast here since va_arg only
                   takes fully promoted types */
                c = (char)va_arg(ap, int);
                print_c(c);
                break;
            case '%':
                print_c('%');
                break;
            default:
                print_c('%');
                print_c(c);
            }
        }
        else
        {
            print_c(c);
        }
        c = *fmt++;
    }
    va_end(ap);
}
#endif