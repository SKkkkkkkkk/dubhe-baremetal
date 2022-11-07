#include <stdio.h>
#include <stdint.h>
#include "i2c.h"
#include "i2c_drv.h"
#include "i2c_slave.h"
// #include "systimer.h"
// #include "irq_ctrl.h"
#include "i2c_debug.h"

// #ifndef NO_SYS
// #include <FreeRTOS.h>
// #include <semphr.h>

// static SemaphoreHandle_t g_mutex[5] = {NULL};
// #endif

static struct i2c_adapter *g_handle[5] = {NULL};

static struct i2c_adapter *get_i2c_handle(uint32_t i2cbus)
{
	if (i2cbus > 4) {
		return NULL;
	} else {
		return g_handle[i2cbus];
	}
}

#if 0
static void set_i2c_iomux(int i2cbus)
{
	if (i2cbus == 0) {
		pin_set_iomux(GROUP_GPIO2, 31, 0);
		pin_set_iomux(3, 0, 0);
	} else if (i2cbus == 1) {
		pin_set_iomux(3, 1, 0);
		pin_set_iomux(3, 2, 0);
	} else if (i2cbus == 2) {
		pin_set_iomux(GROUP_GPIO2, 27, 1);
		pin_set_iomux(GROUP_GPIO2, 28, 1);
	} else if (i2cbus == 3) {
		pin_set_iomux(GROUP_GPIO1, 26, 1);
		pin_set_iomux(GROUP_GPIO1, 27, 1);
	} else if (i2cbus == 4) {
		pin_set_iomux(GROUP_GPIO1, 2, 0);
		pin_set_iomux(GROUP_GPIO1, 3, 0);
	}
}

static void clear_i2c_iomux(int i2cbus)
{
	if (i2cbus == 0) {
		pin_set_iomux(GROUP_GPIO2, 31, 3);
		pin_set_iomux(3, 0, 3);
	} else if (i2cbus == 1) {
		pin_set_iomux(3, 1, 3);
		pin_set_iomux(3, 2, 3);
	} else if (i2cbus == 2) {
		pin_set_iomux(GROUP_GPIO2, 27, 3);
		pin_set_iomux(GROUP_GPIO2, 28, 3);
	} else if (i2cbus == 3) {
		pin_set_iomux(GROUP_GPIO1, 26, 3);
		pin_set_iomux(GROUP_GPIO1, 27, 3);
	} else if (i2cbus == 4) {
		pin_set_iomux(GROUP_GPIO1, 2, 3);
		pin_set_iomux(GROUP_GPIO1, 3, 3);
	}
}
#endif

static int check_i2cbus(int i2cbus)
{
	if (i2cbus < 0 || i2cbus > 4) {
		i2c_debug_err("%s[%d] error: i2c bus Invalid \n", __func__, __LINE__);
		return -1;
	}
	return 0;
}

static void set_i2c_handle(int i2cbus, struct i2c_adapter *i2c_ada)
{
	if (i2cbus < 5) {
		g_handle[i2cbus] = i2c_ada;
	}
}

#if 0
static void i2c0_irq_handle(void)
{
	struct i2c_adapter *i2c_adp = get_i2c_handle(0);
	if (i2c_adp != NULL && i2c_adp->irq_handle != NULL) {
		if (i2c_adp->irq_handle(i2c_adp) != 1) {
			i2c_function_err(i2c_adp->irq_handle(i2c_adp));
		}
	}
}

static void i2c1_irq_handle(void)
{
	struct i2c_adapter *i2c_adp = get_i2c_handle(1);
	if (i2c_adp != NULL && i2c_adp->irq_handle != NULL) {
		if (i2c_adp->irq_handle(i2c_adp) != 1) {
			i2c_function_err(i2c_adp->irq_handle(i2c_adp));
		}
	}
}

static void i2c2_irq_handle(void)
{
	struct i2c_adapter *i2c_adp = get_i2c_handle(2);
	if (i2c_adp != NULL && i2c_adp->irq_handle != NULL) {
		if (i2c_adp->irq_handle(i2c_adp) != 1) {
			i2c_function_err(i2c_adp->irq_handle(i2c_adp));
		}
	}
}

static void i2c3_irq_handle(void)
{
	struct i2c_adapter *i2c_adp = get_i2c_handle(3);
	if (i2c_adp != NULL && i2c_adp->irq_handle != NULL) {
		if (i2c_adp->irq_handle(i2c_adp) != 1) {
			i2c_function_err(i2c_adp->irq_handle(i2c_adp));
		}
	}
}

static void i2c4_irq_handle(void)
{
	struct i2c_adapter *i2c_adp = get_i2c_handle(4);
	if (i2c_adp != NULL && i2c_adp->irq_handle != NULL) {
		if (i2c_adp->irq_handle(i2c_adp) != 1) {
			i2c_function_err(i2c_adp->irq_handle(i2c_adp));
		}
	}
}

static int i2c_irq_register(uint32_t irq_num, uint32_t i2cbus)
{
	switch (i2cbus) {
	case 0:
		IRQ_SetHandler(irq_num, i2c0_irq_handle);
		break;
	case 1:
		IRQ_SetHandler(irq_num, i2c1_irq_handle);
		break;
	case 2:
		IRQ_SetHandler(irq_num, i2c2_irq_handle);
		break;
	case 3:
		IRQ_SetHandler(irq_num, i2c3_irq_handle);
		break;
	case 4:
		IRQ_SetHandler(irq_num, i2c4_irq_handle);
		break;
	default:
		return -1;
	}

	IRQ_SetPriority(irq_num, 15 << 3);
	IRQ_Enable(irq_num);
	return 0;
}
#endif

int i2c_master_init(int i2cbus)
{
	if (check_i2cbus(i2cbus) != 0) {
		i2c_parem_invalid();
		return -1;
	}

	// set_i2c_iomux(i2cbus);

	struct i2c_adapter *i2c_master = get_i2c_handle(i2cbus);
	if (i2c_master != NULL && i2c_master->is_slave == false) {
		return 0;
	}
	if (i2c_master == NULL) {
		i2c_master = ape_i2c_init(i2cbus);
		if (i2c_master == NULL) {
			i2c_function_err(ape_i2c_init);
			// clear_i2c_iomux(i2cbus);
			return -1;
		}
	}
	i2c_master->is_slave = false;

	set_i2c_handle(i2cbus, i2c_master);

	// int ret = i2c_irq_register(i2c_master->irq_num, i2cbus);
	// if (ret != 0) {
	// 	i2c_function_err(i2c_irq_register);
	// 	ape_i2c_uninit(i2c_master);
	// 	set_i2c_handle(i2cbus, NULL);
	// 	clear_i2c_iomux(i2cbus);
	// 	return -1;
	// }

	i2c_master->init(i2c_master);

	return 0;
}

int i2c_config_control(int i2cbus, i2c_control *control)
{
	if (control == NULL) {
		i2c_pointer_invalid(control);
		return -1;
	}

	if (check_i2cbus(i2cbus) != 0) {
		i2c_parem_invalid();
		return -1;
	}

	struct i2c_adapter *i2cm = get_i2c_handle(i2cbus);
	if (i2cm == NULL) {
		i2c_pointer_invalid(i2cm);
		return -1;
	}

	if (control->fifo_tl != 0) {
		i2cm->fifo_tl = control->fifo_tl;
	}

	if (control->one_adata_len != 0) {
		i2cm->one_adata_len = control->one_adata_len;
	}

	if (control->timeout != 0) {
		i2cm->timeout = control->timeout;
	}

	if (control->speed != 0) {
		i2cm->speed = control->speed;
	}

	i2cm->addr_10bit = control->addr_10bit;

	if (control->slaveaddr != 0) {
		i2cm->slaveaddr = control->slaveaddr;
	}
	// i2cm->hwadapnr = i2cbus;

	if (i2cm->init == NULL) {
		i2c_pointer_invalid(i2cm->init);
		return -1;
	}

	i2cm->init(i2cm);

	return 0;
}

int i2c_master_probe(int i2cbus, uint16_t slaveaddr)
{
	int ret = 0;
	if (check_i2cbus(i2cbus) != 0) {
		i2c_parem_invalid();
		return -1;
	}

	struct i2c_adapter *i2cm = get_i2c_handle(i2cbus);
	if (i2cm == NULL || i2cm->probe == NULL || i2cm->is_slave == true) {
		i2c_pointer_invalid(i2cm);
		return -1;
	}

	i2cm->slaveaddr = slaveaddr;

	ret = i2cm->probe(i2cm, slaveaddr);
	if (ret != 0) {
		i2c_function_err(i2cm->read);
		return -1;
	}

// #ifndef NO_SYS
// 	xSemaphoreGive(g_mutex[i2cbus]);
// #endif
	return 0;
}

int i2c_master_transfer(int i2cbus, i2c_msg *msgs, int num)
{
	int i;
	int ret = 0;
	int err_cnt = 0;
	if (check_i2cbus(i2cbus) != 0) {
		i2c_parem_invalid();
		return -1;
	}

	if (msgs == NULL) {
		i2c_parem_invalid();
		return -1;
	}

// #ifndef NO_SYS
// 	xSemaphoreTake(g_mutex[i2cbus], portMAX_DELAY);
// #endif
	struct i2c_adapter *i2cm = get_i2c_handle(i2cbus);
	if (i2cm == NULL || i2cm->is_slave == true) {
		i2c_pointer_invalid(i2cm);
		ret = -1;
		goto ext;
	}

	for (i = 0; i < num; i++) {
		if (msgs[i].buf == NULL || msgs[i].buf_len == 0) {
			i2c_parem_invalid();
			err_cnt++;
			continue;
		}

		if ((msgs[i].flags == RT_I2C_RD) && (i2cm->read != NULL)) {
			ret = i2cm->read(i2cm, msgs[i].addr, msgs[i].reg_addr, msgs[i].alen, msgs[i].buf,
					 msgs[i].buf_len);
			if (ret != 0) {
				i2c_function_err(i2cm->read);
				err_cnt++;
			}
		} else if ((msgs[i].flags == RT_I2C_WR) && (i2cm->write != NULL)) {
			ret = i2cm->write(i2cm, msgs[i].addr, msgs[i].reg_addr, msgs[i].alen, msgs[i].buf,
					  msgs[i].buf_len);
			if (ret != 0) {
				i2c_function_err(i2cm->write);
				err_cnt++;
			}
		} else {
			i2c_parem_invalid();
			ret = -1;
			break;
		}
	}

ext:
// #ifndef NO_SYS
// 	xSemaphoreGive(g_mutex[i2cbus]);
// #endif

	if (err_cnt != 0) {
		i2c_debug_err("i2c transfer error!\n");
		ret = -1;
	}

	return ret;
}

int i2c_master_send(int i2cbus, i2c_msg *msg)
{
	int ret;
	if (msg == NULL || msg->buf == NULL || msg->buf_len == 0) {
		i2c_parem_invalid();
		return -1;
	}

	if (check_i2cbus(i2cbus) != 0) {
		i2c_parem_invalid();
		return -1;
	}

	struct i2c_adapter *i2cm = get_i2c_handle(i2cbus);
	if (i2cm == NULL || i2cm->write == NULL || i2cm->is_slave == true) {
		i2c_pointer_invalid(i2cm);
		ret = -1;
		goto ext;
	}

	ret = i2cm->write(i2cm, msg->addr, msg->reg_addr, msg->alen, msg->buf, msg->buf_len);
	if (ret != 0) {
		i2c_function_err(i2cm->write);
		goto ext;
	}

ext:
	return ret;
}

int i2c_master_recv(int i2cbus, i2c_msg *msg)
{
	int ret;
	if (msg == NULL || msg->buf == NULL || msg->buf_len == 0) {
		i2c_parem_invalid();
		return -1;
	}

	if (check_i2cbus(i2cbus) != 0) {
		i2c_parem_invalid();
		return -1;
	}

// #ifndef NO_SYS
// 	xSemaphoreTake(g_mutex[i2cbus], portMAX_DELAY);
// #endif
	struct i2c_adapter *i2cm = get_i2c_handle(i2cbus);
	if (i2cm == NULL || i2cm->read == NULL || i2cm->is_slave == true) {
		i2c_pointer_invalid(i2cm);
		ret = -1;
		goto ext;
	}

	ret = i2cm->read(i2cm, msg->addr, msg->reg_addr, msg->alen, msg->buf, msg->buf_len);
	if (ret != 0) {
		i2c_function_err(i2cm->read);
		goto ext;
	}

ext:
// #ifndef NO_SYS
// 	xSemaphoreGive(g_mutex[i2cbus]);
// #endif

	return ret;
}

void i2c_master_deinit(int i2cbus)
{
	struct i2c_adapter *i2cm = get_i2c_handle(i2cbus);
	if (i2cm != NULL) {
		ape_i2c_uninit(i2cm);
		set_i2c_handle(i2cbus, NULL);
		// clear_i2c_iomux(i2cbus);
	}
}

#ifdef SUPPORT_I2C_SLAVE

int i2c_slave_init(int i2cbus, uint32_t slaveaddr)
{
	if (check_i2cbus(i2cbus) != 0) {
		i2c_parem_invalid();
		return -1;
	}

	// set_i2c_iomux(i2cbus);

	struct i2c_adapter *i2c_slave = get_i2c_handle(i2cbus);
	if (i2c_slave != NULL && i2c_slave->is_slave && i2c_slave->slaveaddr == slaveaddr) {
		return 0;
	}

	if (i2c_slave == NULL) {
		i2c_slave = ape_i2c_init(i2cbus);
		if (i2c_slave == NULL) {
			i2c_function_err(ape_i2c_init);
			// clear_i2c_iomux(i2cbus);
			return -1;
		}
	}

	i2c_slave->slaveaddr = slaveaddr;
	i2c_slave->is_slave = true;
	i2c_slave->irq_handle = i2c_dw_irq_handler_slave;
	i2c_slave->irq_num = (IIC0_IRQn + i2cbus);

	set_i2c_handle(i2cbus, i2c_slave);

	int ret = i2c_irq_register(i2c_slave->irq_num, i2cbus);
	if (ret != 0) {
		i2c_function_err(i2c_irq_register);
		ape_i2c_uninit(i2c_slave);
		set_i2c_handle(i2cbus, NULL);
		// clear_i2c_iomux(i2cbus);
		return -1;

	}

	i2c_slave->init(i2c_slave);
	return 0;
}

void i2c_slave_deinit(int i2cbus)
{
	struct i2c_adapter *i2cs = get_i2c_handle(i2cbus);
	if (i2cs != NULL) {

		ape_i2c_uninit(i2cs);
		set_i2c_handle(i2cbus, NULL);
		// clear_i2c_iomux(i2cbus);
	}
}
#endif
