#ifndef __I2C_WO_REGS_H__
#define __I2C_WO_REGS_H__

#include <stdint.h>
#if defined M3
#include "_cm3_chip_define.h"
#else
#include "_ca55_chip_define.h"
#endif

typedef struct {
    __IOM unsigned int START;             /* 0x00 */
    __IOM unsigned int DELAY;             /* 0x04 */
    __IOM unsigned int FIFO_SOFT_RST;     /* 0x08 */
    __IOM unsigned int DEVICE_ADDR;       /* 0x0c */
    __IOM unsigned int TX_DATA;           /* 0x10 */
    __IOM unsigned int SCL_HCNT;          /* 0x14 */
    __IOM unsigned int SCL_LCNT;          /* 0x18 */
    __IOM unsigned int STATUS;            /* 0x1c */
    __IOM unsigned int STATUS_FIFO_LEVEL; /* 0x20 */
    __IOM unsigned int DELAY_UNIT;		  /* 0x24 */
    __IOM unsigned int APPOWER_STATUS_DISABLE;		  /* 0x28 */

} I2C_WO_TypeDef;

#define I2C_WO ((I2C_WO_TypeDef *) I2C_WO_BASE)

////////////////////////////////////////////////////////////////
// baisc reg info                                             //
////////////////////////////////////////////////////////////////

#define I2C_WO_START_ADDR                (I2C_WO_BASE+0x00)

#define I2C_WO_START_EN_ADDR             (I2C_WO_BASE+0x00)

#define I2C_WO_DELAY_ADDR                (I2C_WO_BASE+0x04)

#define I2C_WO_FIFO_SOFT_RST_ADDR        (I2C_WO_BASE+0x08)

#define I2C_WO_FIFO_SOFT_RST_CLR_ADDR    (I2C_WO_BASE+0x08)

#define I2C_WO_DEVICE_ADDR_ADDR          (I2C_WO_BASE+0x0c)

#define I2C_WO_TX_DATA_ADDR              (I2C_WO_BASE+0x10)

#define I2C_WO_TX_DATA_EN_ADDR           (I2C_WO_BASE+0x10)

#define I2C_WO_SCL_HCNT_ADDR             (I2C_WO_BASE+0x14)

#define I2C_WO_SCL_LCNT_ADDR             (I2C_WO_BASE+0x18)

#define I2C_WO_STATUS_SEND_DONE_ADDR     (I2C_WO_BASE+0x1c)
#define I2C_WO_STATUS_SEND_DONE_LSB      0
#define I2C_WO_STATUS_SEND_DONE_MASK     0x00000001

#define I2C_WO_STATUS_NOACK_ADDR         (I2C_WO_BASE+0x1c)
#define I2C_WO_STATUS_NOACK_LSB          1
#define I2C_WO_STATUS_NOACK_MASK         0x00000002

#define I2C_WO_STATUS_FIFO_LEVEL_ADDR    (I2C_WO_BASE+0x20)

////////////////////////////////////////////////////////////////
// reg access                                                 //
////////////////////////////////////////////////////////////////
#define I2C_WO_REG32(name)               (*(volatile uint32_t*)name##_ADDR)

#define I2C_WO_GET_START                 I2C_WO_REG32(I2C_WO_START)
#define I2C_WO_SET_START(x)              I2C_WO_REG32(I2C_WO_START) = (uint32_t)(x)
#define I2C_WO_SET_START_EN(x)           I2C_WO_REG32(I2C_WO_START_EN) = (uint32_t)(x)
#define I2C_WO_GET_DELAY                 I2C_WO_REG32(I2C_WO_DELAY)
#define I2C_WO_SET_DELAY(x)              I2C_WO_REG32(I2C_WO_DELAY) = (uint32_t)(x)
#define I2C_WO_GET_FIFO_SOFT_RST         I2C_WO_REG32(I2C_WO_FIFO_SOFT_RST)
#define I2C_WO_SET_FIFO_SOFT_RST(x)      I2C_WO_REG32(I2C_WO_FIFO_SOFT_RST) = (uint32_t)(x)
#define I2C_WO_SET_FIFO_SOFT_RST_CLR(x)  I2C_WO_REG32(I2C_WO_FIFO_SOFT_RST_CLR) = (uint32_t)(x)
#define I2C_WO_GET_DEVICE_ADDR           I2C_WO_REG32(I2C_WO_DEVICE_ADDR)
#define I2C_WO_SET_DEVICE_ADDR(x)        I2C_WO_REG32(I2C_WO_DEVICE_ADDR) = (uint32_t)(x)
#define I2C_WO_GET_TX_DATA               I2C_WO_REG32(I2C_WO_TX_DATA)
#define I2C_WO_SET_TX_DATA(x)            I2C_WO_REG32(I2C_WO_TX_DATA) = (uint32_t)(x)
#define I2C_WO_SET_TX_DATA_EN(x)         I2C_WO_REG32(I2C_WO_TX_DATA_EN) = (uint32_t)(x)
#define I2C_WO_GET_SCL_HCNT              I2C_WO_REG32(I2C_WO_SCL_HCNT)
#define I2C_WO_SET_SCL_HCNT(x)           I2C_WO_REG32(I2C_WO_SCL_HCNT) = (uint32_t)(x)
#define I2C_WO_GET_SCL_LCNT              I2C_WO_REG32(I2C_WO_SCL_LCNT)
#define I2C_WO_SET_SCL_LCNT(x)           I2C_WO_REG32(I2C_WO_SCL_LCNT) = (uint32_t)(x)
#define I2C_WO_GET_STATUS_FIFO_LEVEL     I2C_WO_REG32(I2C_WO_STATUS_FIFO_LEVEL)

////////////////////////////////////////////////////////////////
// reg bits access                                            //
////////////////////////////////////////////////////////////////
#define I2C_WO_GET_REG32_BITS(name)      ((I2C_WO_REG32(name) & name##_MASK) >> name##_LSB)
#define I2C_WO_SET_REG32_BITS(name,x)    do { \
    uint32_t val = I2C_WO_REG32(name); \
    val &= ~name##_MASK; \
    val |= ((x) << name##_LSB) & name##_MASK; \
    I2C_WO_REG32(name) = val; \
} while(0)

#define I2C_WO_GET_STATUS_SEND_DONE      I2C_WO_GET_REG32_BITS(I2C_WO_STATUS_SEND_DONE)
#define I2C_WO_GET_STATUS_NOACK          I2C_WO_GET_REG32_BITS(I2C_WO_STATUS_NOACK)

#endif // __I2C_WO_REGS_H__
