#include "dw_apb_uart.h"
// #include "ape1210.h"
// #include "irq_ctrl.h"

//#define ENABLE_UART_INTERRUPT

#ifdef ENABLE_UART_INTERRUPT
#define UART0_INT_NUM UART0_IRQn
#define UART1_INT_NUM UART1_IRQn
#define UART2_INT_NUM UART2_IRQn
#define UART3_INT_NUM UART3_IRQn

#define ENABLE_UART_RECV_DATA_INT     BIT(0)
#define ENABLE_UART_TRANS_EMPTY_INT   BIT(1)
#endif
#define writel(d, a) (*(volatile uint32_t *)(a) = (d))
#define readl(a) (*(volatile uint32_t *)(a))

#define UART_USED UART0_BASE

#define UART_FIFO_DEPTH 64

#define UART_LCR_BKSE   0x80        /* Bank select enable */
#define UART_LCR_8N1	0x03

#define UART_STOPBITS_1   0  /* decide the data frame has 1 bit stop bit. */
#define UART_STOPBITS_2   1  /* decide the data frame has 1 bit stop bit. */


#define UART__PARITY_NONE       0
#define UART_PARITY_ENABLE      1
#define UART_PARITY_UNENABLE    0

#define UART_PARITY_ODD         1
#define UART_PARITY_EVEN        3

#define UART_TRANS_FIFO_LEVEL   0x3
#define UART_RCV_FIFO_LEVEL     0x3

#define UART_FIFO_ENABLE        1   /* the UART FIFO is ENABLE. */
#define UART_FIFO_UNENABLE      0   /* the UART FIFO is UNENABLE. */

#define UART_DMA_MODE1          1     /* the UART DMA mode is 1. */
#define UART_DMA_MODE0          0     /* the UART DMA mode is 0. */

#define SEEHI_UART_INT_ELCOLR   4     /* interrupt the enable bit */

typedef enum {
    UART_RCVR_TRIGGER_ONECH = 0x0, /* definitions the receiver FIFO has one character available, the interrupt is generated. */
    UART_RCVR_TRIGGER_QUARTER_FULL = 0x1, /* definitions the receiver FIFO is 1/4 full, the interrupt is generated. */
    UART_RCVR_TRIGGER_HALF_FULL = 0x2, /* definitions the receiver FIFO is 1/2 full, the interrupt is generated. */
    UART_RCVR_TRIGGER_TWO_LESS_FULL = 0x3, /* definitions the receiver FIFO is two characters less than full, the interrupt is generated. */
} uart_rcvr_trigger;

typedef enum {
    UART_TXEMPTY_TRIGGER_EMPTY = 0x0, /* that the transmit FIFO is empty */
    UART_TXEMPTY_TRIGGER_TWO = 0x1, /* when there are two characters in TX FIFO */
    UART_TXEMPTY_TRIGGER_QUARTER_FULL = 0x2, /* that the transmit FIFO is 1/4 full */
    UART_TXEMPTY_TRIGGER_HALF_FULL = 0x3, /* that the transmit FIFO is 1/2 full */
} uart_tx_emty_trigger; /* the THRE interrupt is generated. */

typedef enum {
    UART_WORDLENGTN_5B = 0x0,
    UART_WORDLENGTN_6B = 0x1,
    UART_WORDLENGTN_7B = 0x2,
    UART_WORDLENGTN_8B = 0x3, /* decide the data frame has 8 bit per char. */
} uart_word_lengtn;

//-------------------------------------------------------------------------------
//define address offset
//-------------------------------------------------------------------------------

typedef DW_APB_UART_TypeDef uart_reg;

static unsigned int g_uart_used = UART_USED;

static unsigned int get_uart_addr(seehi_uart_no uart_no)
{
  unsigned int uart_addr = 0;
  switch(uart_no) {
    case SEEHI_UART0:
      uart_addr = UART0_BASE;
      break;
    case SEEHI_UART1:
      uart_addr = UART1_BASE;
      break;
    case SEEHI_UART2:
      uart_addr = UART2_BASE;
      break;
    case SEEHI_UART3:
      uart_addr = UART3_BASE;
      break;
    default:
      break;
  }
  return uart_addr;
}

#ifdef ENABLE_UART_INTERRUPT
#define UART_DATA_READY_BIT BIT(0)

extern void serial_isr(seehi_uart_no uart_no);

static void _uart_handler(seehi_uart_no uart_no)
{
	uint32_t reg = 0u;
	uart_reg *uart_reg_ptr = (uart_reg *)(uintptr_t)get_uart_addr(0);

	if (uart_reg_ptr == NULL) 
	{
		return;
	}
	serial_isr(uart_no);
}

static void uart0_irq_handler(void)
{
	_uart_handler(SEEHI_UART0);
}
static void uart1_irq_handler(void)
{
	_uart_handler(SEEHI_UART1);
}
static void uart2_irq_handler(void)
{
	_uart_handler(SEEHI_UART2);
}
static void uart3_irq_handler(void)
{
	_uart_handler(SEEHI_UART3);
}
#endif
int seehi_uart_config_baudrate(unsigned long baudrate, unsigned long uart_clk, seehi_uart_no uart_no)
{
    /*
     * Baud Rate Divisor =Serial Clock Frequency/(16 *Required Baud Rate) = BRDI + BRDF
     * BRDI - Integer part of the divisor
     * BRDI - Integer part of the divisor
     */
    unsigned long div;
    float div_f;
    int dlf_value=0;
    unsigned int uart_addr;
    unsigned int reg_val = 0u;
    (void)reg_val;
#ifdef ENABLE_UART_INTERRUPT
	IRQHandler_t handler = NULL;
	IRQn_ID_t irq_nu = 0;
#endif

    if(baudrate == 0 || uart_clk == 0) {
        return -1;
    }

    div = (unsigned long)(uart_clk / baudrate) ;

    //if(div>=16){//realize the fraction baud rate
    div_f=((float)uart_clk / (16 * baudrate)) - (int)((float)uart_clk / (16 * baudrate));
    dlf_value=(int)(div_f * 16);
    if((div_f * 16.0 - dlf_value) > 0.5) {
        dlf_value = dlf_value + 1;
    }

    div = (div >> 4) ;//+ ((div & 0x00000008) >> 3) ;

    uart_addr = get_uart_addr(uart_no);
    if (uart_addr == 0) {
      return -2;
    }

    uart_reg *uart_reg_ptr = (uart_reg *)(uintptr_t)uart_addr;

    writel((readl(&uart_reg_ptr->LCR) | 0x80), &uart_reg_ptr->LCR);
    writel(dlf_value, &uart_reg_ptr->DLF);
    writel(div & 0xff, &uart_reg_ptr->DLL);
    writel(div >> 8, &uart_reg_ptr->DLH);
    writel(readl(&uart_reg_ptr->LCR) & 0xffffff7f, &uart_reg_ptr->LCR);

    /* configure uart line control */
    unsigned int ul_config = (UART_PARITY_UNENABLE << 3) | (UART_STOPBITS_1 << 2) | UART_WORDLENGTN_8B;
    writel(ul_config, &uart_reg_ptr->LCR);

    /* enable UART0 FIFO, set RCVR Triggle threshold one byte */
    unsigned int fifo_config = ((UART_RCVR_TRIGGER_ONECH & 0x3) << 6) | ((UART_TXEMPTY_TRIGGER_QUARTER_FULL & 0x3) << 4) | (0x6 | UART_FIFO_ENABLE);
    writel(fifo_config, &uart_reg_ptr->FCR);
  #ifdef ENABLE_UART_INTERRUPT
    reg_val = readl(&uart_reg_ptr->IER);
    #ifdef ENABLE_TRANS_INT
    reg_val |= ENABLE_UART_TRANS_EMPTY_INT;
    #endif
    reg_val |= ENABLE_UART_RECV_DATA_INT;
    writel(reg_val, &uart_reg_ptr->IER);
	
	switch (uart_no)
	{
	case SEEHI_UART0:
		handler = uart0_irq_handler;
		irq_nu  = UART0_INT_NUM;
		break;
	case SEEHI_UART1:
		handler = uart1_irq_handler;
		irq_nu  = UART1_INT_NUM;
		break;
	case SEEHI_UART2:
		handler = uart2_irq_handler;
		irq_nu  = UART2_INT_NUM;
		break;
	case SEEHI_UART3:
		handler = uart3_irq_handler;
		irq_nu  = UART3_INT_NUM;
		break;
	default:
		return -3;
		break;
	}
    IRQ_SetHandler(irq_nu, handler);
    IRQ_SetPriority(irq_nu, 0 << 3);
    IRQ_Enable(irq_nu);
  #endif
    g_uart_used = uart_addr;
    return 0;
}

#ifdef ENABLE_UART_INTERRUPT
void seehi_uart_int_enable_old(seehi_uart_no uart_no)
{
    uart_reg *uart_reg_ptr;
    unsigned int reg_val = 0u;

    if (get_uart_addr(uart_no) == 0) {
      return;
    }
    uart_reg_ptr = (uart_reg *)(uintptr_t)get_uart_addr(uart_no);
    if (uart_reg_ptr == 0) {
      return;
    }
    reg_val = readl(&uart_reg_ptr->IER);
  #ifdef ENABLE_TRANS_INT
    reg_val |= ENABLE_UART_TRANS_EMPTY_INT;
  #endif
    reg_val |= ENABLE_UART_RECV_DATA_INT;
    writel(reg_val, &uart_reg_ptr->IER);
}

void seehi_uart_int_disable_old(seehi_uart_no uart_no)
{
    uart_reg *uart_reg_ptr;
    unsigned int reg_val = 0u;

    if (get_uart_addr(uart_no) == 0) {
      return;
    }
    uart_reg_ptr = (uart_reg *)(uintptr_t)get_uart_addr(uart_no);
    if (uart_reg_ptr == 0) {
      return;
    }
    reg_val = readl(&uart_reg_ptr->IER);
  #ifdef ENABLE_TRANS_INT
    reg_val &= ~ENABLE_UART_TRANS_EMPTY_INT;
  #endif
    reg_val &= ~ENABLE_UART_RECV_DATA_INT;
    writel(reg_val, &uart_reg_ptr->IER);
}
#else
// void seehi_uart_int_enable_old(seehi_uart_no uart_no){LOG_E("uart interrupt not enable");};
// void seehi_uart_int_disable_old(seehi_uart_no uart_no){};
#endif

/* 阻塞方式 */
int uart_getchar(seehi_uart_no uart_no)
{
    uart_reg *uart_reg_ptr = (uart_reg *)(uintptr_t)get_uart_addr(uart_no);
    if (uart_reg_ptr == 0) {
      return -1;
    }

    while ((readl(&uart_reg_ptr->LSR) & 0x1) != 0x1);

    return readl(&uart_reg_ptr->RBR);
}

/* 非阻塞方式 */
int uart_getc(seehi_uart_no uart_no)
{
    int ch = -1;
    uart_reg *uart_reg_ptr = (uart_reg *)(uintptr_t)get_uart_addr(uart_no);
    if (uart_reg_ptr == 0) {
      return -1;
    }

    if  ((readl(&uart_reg_ptr->LSR) & 0x1) == 0x1)
    {
      ch = readl(&uart_reg_ptr->RBR);
    }

    return ch;
}

int uart_sendchar(seehi_uart_no uart_no, const int c)
{
    uart_reg *uart_reg_ptr = (uart_reg *)(uintptr_t)get_uart_addr(uart_no);
    if (uart_reg_ptr == 0) {
      return -1;
    }

    /* wait until there is space in the fifo */
    while ((readl(&uart_reg_ptr->LSR) & 0x20) == 0);

    writel((unsigned int)c, &uart_reg_ptr->THR);

    while ((readl(&uart_reg_ptr->LSR) & 0x40) == 0);

    return c;
}

static int uart_tstc(unsigned int base)
{
    uart_reg *uart_reg_ptr = (uart_reg *)(uintptr_t)base;
    return ((readl(&uart_reg_ptr->LSR) & 0x1) != 0);
}

// int putchar(int s)
// {
//     uart_sendchar(g_uart_used, s);
//     return s;
// }

// int getchar()
// {
//     return uart_getchar(g_uart_used);
// }

int tstc()
{
    return uart_tstc(g_uart_used);
}
