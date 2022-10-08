#ifdef SUPPORT_I2C_SLAVE
#include "i2c.h"
#include "i2c_drv.h"
#include "i2c_reg.h"
#include "i2c_debug.h"
/*
#define CFG_TOP_BASE  0xFF4C8000
#define CFG_GPIO4A_IOMUX      CFG_TOP_BASE+0x30
*/
#define I2C_RESTART_EN
#define DW_IC_STATUS_SLAVE_ACTIVITY (1 << 6)
#define DW_IC_INTR_RX_UNDER (1 << 0)
#define DW_IC_INTR_RX_OVER (1 << 1)
#define DW_IC_INTR_RX_FULL (1 << 2)
#define DW_IC_INTR_TX_OVER (1 << 3)
#define DW_IC_INTR_TX_ABRT (1 << 6)
#define DW_IC_INTR_RD_REQ (1 << 5)
#define DW_IC_INTR_RX_DONE (1 << 7)
#define DW_IC_INTR_ACTIVITY (1 << 8)
#define DW_IC_INTR_STOP_DET (1 << 9)
#define DW_IC_INTR_START_DET (1 << 10)
#define DW_IC_INTR_GEN_CALL (1 << 11)
enum i2c_slave_event_e {
	I2C_SLAVE_READ_REQUESTED,
	I2C_SLAVE_WRITE_REQUESTED,
	I2C_SLAVE_READ_PROCESSED,
	I2C_SLAVE_WRITE_RECEIVED,
	I2C_SLAVE_STOP,
};

struct eeprom_data {
    bool first_write;
    uint32_t buffer_idx;
    uint8_t buffer[64];
};

typedef enum {
    STATUS_IDLE,
    STATUS_READ_IN_PROGRESS,
    STATUS_WRITE_IN_PROGRESS
} slave_run_status;

struct eeprom_data g_eeprom_data[5] = {0};

slave_run_status g_slave_status[5] = {STATUS_IDLE};

static uint32_t i2c_dw_read_clear_intrbits_slave(i2c_regs *i2c)
{
	uint32_t stat;

	/*
	 * The IC_INTR_STAT register just indicates "enabled" interrupts.
	 * Ths unmasked raw version of interrupt status bits are available
	 * in the IC_RAW_INTR_STAT register.
	 *
	 * That is,
	 *   stat = dw_readl(IC_INTR_STAT);
	 * equals to,
	 *   stat = dw_readl(IC_RAW_INTR_STAT) & dw_readl(IC_INTR_MASK);
	 *
	 * The raw version might be useful for debugging purposes.
	 */
	stat = readl(&i2c->IC_INTR_STAT);

	/*
	 * Do not use the IC_CLR_INTR register to clear interrupts, or
	 * you'll miss some interrupts, triggered during the period from
	 * dw_readl(IC_INTR_STAT) to dw_readl(IC_CLR_INTR).
	 *
	 * Instead, use the separately-prepared IC_CLR_* registers.
	 */
	if (stat & DW_IC_INTR_TX_ABRT)
		readl(&i2c->IC_CLR_TX_ABRT);
	if (stat & DW_IC_INTR_RX_UNDER)
		readl(&i2c->IC_CLR_RX_UNDER);
	if (stat & DW_IC_INTR_RX_OVER)
		readl(&i2c->IC_CLR_RX_OVER);
	if (stat & DW_IC_INTR_TX_OVER)
		readl(&i2c->IC_CLR_TX_OVER);
	if (stat & DW_IC_INTR_RX_DONE)
		readl(&i2c->IC_CLR_RX_DONE);
	if (stat & DW_IC_INTR_ACTIVITY)
		readl(&i2c->IC_CLR_ACTIVITY);
	if (stat & DW_IC_INTR_STOP_DET)
		readl(&i2c->IC_CLR_STOP_DET);
	if (stat & DW_IC_INTR_START_DET)
		readl(&i2c->IC_CLR_START_DET);
	if (stat & DW_IC_INTR_GEN_CALL)
		readl(&i2c->IC_CLR_GEN_CALL);

	return stat;
}

static int i2c_slave_event(uint32_t i2cbus, enum i2c_slave_event_e event, uint8_t *val)
{
	struct eeprom_data *eeprom = NULL;//g_eeprom_data;

	eeprom = &g_eeprom_data[i2cbus];
	switch (event) {
	case I2C_SLAVE_WRITE_RECEIVED:
		if (eeprom->first_write) {
			eeprom->buffer_idx = *val;
			eeprom->first_write = false;
            i2c_debug_info("%s[%d] val = 0x%x\n", __FUNCTION__, __LINE__, *val);
		} else {
			eeprom->buffer[eeprom->buffer_idx++] = *val;
            i2c_debug_info("%s[%d] val = 0x%x\n", __FUNCTION__, __LINE__, *val);
		}
        i2c_debug_info("%s[%d] val = 0x%x\n", __FUNCTION__, __LINE__, *val);
		break;

	case I2C_SLAVE_READ_PROCESSED:
		/* The previous byte made it to the bus, get next one */
		eeprom->buffer_idx++;
		/* fallthrough */
	case I2C_SLAVE_READ_REQUESTED:
		//spin_lock(&eeprom->buffer_lock);
		*val = 0xaa;//eeprom->buffer[eeprom->buffer_idx];
		//spin_unlock(&eeprom->buffer_lock);
		/*
		 * Do not increment buffer_idx here, because we don't know if
		 * this byte will be actually used. Read Linux I2C slave docs
		 * for details.
		 */
		break;

	case I2C_SLAVE_STOP:
	case I2C_SLAVE_WRITE_REQUESTED:
		eeprom->first_write = true;
		break;

	default:
		break;
	}

	return 0;
}

int i2c_dw_irq_handler_slave(struct i2c_adapter *i2c_ad)
{
    uint32_t raw_stat, stat, enabled, tmp;
	uint8_t val = 0, slave_activity;
	uint32_t i2cbus = i2c_ad->hwadapnr;
    i2c_regs *i2c_base = i2c_get_base(i2cbus);

	enabled = readl(&i2c_base->IC_ENABLE);
    raw_stat = readl(&i2c_base->IC_RAW_INTR_STAT);
    tmp = readl(&i2c_base->IC_STATUS);
	slave_activity = ((tmp & DW_IC_STATUS_SLAVE_ACTIVITY) >> 6);

	if (!enabled || !(raw_stat & ~DW_IC_INTR_ACTIVITY) || !i2c_ad->is_slave)
		return 0;

	stat = i2c_dw_read_clear_intrbits_slave(i2c_base);
	i2c_debug_info("%#x STATUS SLAVE_ACTIVITY=%#x : RAW_INTR_STAT=%#x : INTR_STAT=%#x\n",
		enabled, slave_activity, raw_stat, stat);

	if (stat & DW_IC_INTR_RX_FULL) {
		if (g_slave_status[i2cbus] != STATUS_WRITE_IN_PROGRESS) {
			g_slave_status[i2cbus] = STATUS_WRITE_IN_PROGRESS;
			i2c_slave_event(i2cbus, I2C_SLAVE_WRITE_REQUESTED, &val);
		}

		tmp = readl(&i2c_base->IC_CMD_DATA);
		val = tmp;
		if (!i2c_slave_event(i2cbus, I2C_SLAVE_WRITE_RECEIVED, &val)) {
			i2c_debug_info("Byte %X acked!", val);
		}
	}

	if (stat & DW_IC_INTR_RD_REQ) {
		if (slave_activity) {
			tmp = readl(&i2c_base->IC_CLR_RD_REQ);

			g_slave_status[i2cbus] = STATUS_READ_IN_PROGRESS;
			if (!i2c_slave_event(i2cbus, I2C_SLAVE_READ_REQUESTED, &val))
				writel(val, &i2c_base->IC_CMD_DATA);
		}
	}

	if (stat & DW_IC_INTR_RX_DONE) {
		if (!i2c_slave_event(i2cbus, I2C_SLAVE_READ_PROCESSED, &val))
            tmp = readl(&i2c_base->IC_CLR_RX_DONE);
	}

	if (stat & DW_IC_INTR_STOP_DET) {
		        /* slave 作为接收端时，接收最后在FIFO中的数据 */
        while(readl(&i2c_base->IC_RXFLR) != 0) {
            tmp = readl(&i2c_base->IC_CMD_DATA);
            val = tmp;
            if (!i2c_slave_event(i2cbus, I2C_SLAVE_WRITE_RECEIVED, &val)) {
            }
        }

		g_slave_status[i2cbus] = STATUS_IDLE;
		i2c_slave_event(i2cbus, I2C_SLAVE_STOP, &val);
	}

	return 1;
}
#endif