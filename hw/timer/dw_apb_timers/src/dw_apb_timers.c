#include "dw_apb_timers.h"

bool timer_enable(timer_id_t timer_id)
{
	bool ret = true;
	switch (timer_id) {
	case Timerx2_T1:
		TIMERX2->Timer1ControlReg |= TIMER_ENABLE_Msk;
		break;
	case Timerx2_T2:
		TIMERX2->Timer2ControlReg |= TIMER_ENABLE_Msk;
		break;
	case Timerx6_T1:
		TIMERX6->Timer1ControlReg |= TIMER_ENABLE_Msk;
		break;
	case Timerx6_T2:
		TIMERX6->Timer2ControlReg |= TIMER_ENABLE_Msk;
		break;
	case Timerx6_T3:
		TIMERX6->Timer3ControlReg |= TIMER_ENABLE_Msk;
		break;
	case Timerx6_T4:
		TIMERX6->Timer4ControlReg |= TIMER_ENABLE_Msk;
		break;
	case Timerx6_T5:
		TIMERX6->Timer5ControlReg |= TIMER_ENABLE_Msk;
		break;
	case Timerx6_T6:
		TIMERX6->Timer6ControlReg |= TIMER_ENABLE_Msk;
		break;
	default:
		ret = false;
	}
	return ret;
}

bool timer_disable(timer_id_t timer_id)
{
	bool ret = true;
	switch (timer_id) {
	case Timerx2_T1:
		TIMERX2->Timer1ControlReg &= (~TIMER_ENABLE_Msk);
		break;
	case Timerx2_T2:
		TIMERX2->Timer2ControlReg &= (~TIMER_ENABLE_Msk);
		break;
	case Timerx6_T1:
		TIMERX6->Timer1ControlReg &= (~TIMER_ENABLE_Msk);
		break;
	case Timerx6_T2:
		TIMERX6->Timer2ControlReg &= (~TIMER_ENABLE_Msk);
		break;
	case Timerx6_T3:
		TIMERX6->Timer3ControlReg &= (~TIMER_ENABLE_Msk);
		break;
	case Timerx6_T4:
		TIMERX6->Timer4ControlReg &= (~TIMER_ENABLE_Msk);
		break;
	case Timerx6_T5:
		TIMERX6->Timer5ControlReg &= (~TIMER_ENABLE_Msk);
		break;
	case Timerx6_T6:
		TIMERX6->Timer6ControlReg &= (~TIMER_ENABLE_Msk);
		break;
	default:
		ret = false;
	}
	return ret;
}

bool timer_set_mode(timer_id_t timer_id, timer_mode_t timer_mode)
{
	bool ret = true;
	switch (timer_id) {
	case Timerx2_T1:
		if (timer_mode == Mode_Free_Running)
			TIMERX2->Timer1ControlReg &= (~TIMER_MODE_Msk);
		else
			TIMERX2->Timer1ControlReg |= (TIMER_MODE_Msk);
		break;
	case Timerx2_T2:
		if (timer_mode == Mode_Free_Running)
			TIMERX2->Timer2ControlReg &= (~TIMER_MODE_Msk);
		else
			TIMERX2->Timer2ControlReg |= (TIMER_MODE_Msk);
		break;
	case Timerx6_T1:
		if (timer_mode == Mode_Free_Running)
			TIMERX6->Timer1ControlReg &= (~TIMER_MODE_Msk);
		else
			TIMERX6->Timer1ControlReg |= (TIMER_MODE_Msk);
		break;
	case Timerx6_T2:
		if (timer_mode == Mode_Free_Running)
			TIMERX6->Timer2ControlReg &= (~TIMER_MODE_Msk);
		else
			TIMERX6->Timer2ControlReg |= (TIMER_MODE_Msk);
		break;
	case Timerx6_T3:
		if (timer_mode == Mode_Free_Running)
			TIMERX6->Timer3ControlReg &= (~TIMER_MODE_Msk);
		else
			TIMERX6->Timer3ControlReg |= (TIMER_MODE_Msk);
		break;
	case Timerx6_T4:
		if (timer_mode == Mode_Free_Running)
			TIMERX6->Timer4ControlReg &= (~TIMER_MODE_Msk);
		else
			TIMERX6->Timer4ControlReg |= (TIMER_MODE_Msk);
		break;
	case Timerx6_T5:
		if (timer_mode == Mode_Free_Running)
			TIMERX6->Timer5ControlReg &= (~TIMER_MODE_Msk);
		else
			TIMERX6->Timer5ControlReg |= (TIMER_MODE_Msk);
		break;
	case Timerx6_T6:
		if (timer_mode == Mode_Free_Running)
			TIMERX6->Timer6ControlReg &= (~TIMER_MODE_Msk);
		else
			TIMERX6->Timer6ControlReg |= (TIMER_MODE_Msk);
		break;
	default:
		ret = false;
	}
	return ret;
}

bool timer_set_int_mask(timer_id_t timer_id, bool mask)
{
	bool ret = true;
	switch (timer_id) {
	case Timerx2_T1:
		if (!mask)
			TIMERX2->Timer1ControlReg &= (~TIMER_INTERRUPT_MASK_Msk);
		else
			TIMERX2->Timer1ControlReg |= (TIMER_INTERRUPT_MASK_Msk);
		break;
	case Timerx2_T2:
		if (!mask)
			TIMERX2->Timer2ControlReg &= (~TIMER_INTERRUPT_MASK_Msk);
		else
			TIMERX2->Timer2ControlReg |= (TIMER_INTERRUPT_MASK_Msk);
		break;
	case Timerx6_T1:
		if (!mask)
			TIMERX6->Timer1ControlReg &= (~TIMER_INTERRUPT_MASK_Msk);
		else
			TIMERX6->Timer1ControlReg |= (TIMER_INTERRUPT_MASK_Msk);
		break;
	case Timerx6_T2:
		if (!mask)
			TIMERX6->Timer2ControlReg &= (~TIMER_INTERRUPT_MASK_Msk);
		else
			TIMERX6->Timer2ControlReg |= (TIMER_INTERRUPT_MASK_Msk);
		break;
	case Timerx6_T3:
		if (!mask)
			TIMERX6->Timer3ControlReg &= (~TIMER_INTERRUPT_MASK_Msk);
		else
			TIMERX6->Timer3ControlReg |= (TIMER_INTERRUPT_MASK_Msk);
		break;
	case Timerx6_T4:
		if (!mask)
			TIMERX6->Timer4ControlReg &= (~TIMER_INTERRUPT_MASK_Msk);
		else
			TIMERX6->Timer4ControlReg |= (TIMER_INTERRUPT_MASK_Msk);
		break;
	case Timerx6_T5:
		if (!mask)
			TIMERX6->Timer5ControlReg &= (~TIMER_INTERRUPT_MASK_Msk);
		else
			TIMERX6->Timer5ControlReg |= (TIMER_INTERRUPT_MASK_Msk);
		break;
	case Timerx6_T6:
		if (!mask)
			TIMERX6->Timer6ControlReg &= (~TIMER_INTERRUPT_MASK_Msk);
		else
			TIMERX6->Timer6ControlReg |= (TIMER_INTERRUPT_MASK_Msk);
		break;
	default:
		ret = false;
	}
	return ret;
}

bool timer_set_loadcount(timer_id_t timer_id, uint32_t loadcount)
{
	bool ret = true;
	switch (timer_id) {
	case Timerx2_T1:
		TIMERX2->Timer1LoadCount = loadcount;
		break;
	case Timerx2_T2:
		TIMERX2->Timer2LoadCount = loadcount;
		break;
	case Timerx6_T1:
		TIMERX6->Timer1LoadCount = loadcount;
		break;
	case Timerx6_T2:
		TIMERX6->Timer2LoadCount = loadcount;
		break;
	case Timerx6_T3:
		TIMERX6->Timer3LoadCount = loadcount;
		break;
	case Timerx6_T4:
		TIMERX6->Timer4LoadCount = loadcount;
		break;
	case Timerx6_T5:
		TIMERX6->Timer5LoadCount = loadcount;
		break;
	case Timerx6_T6:
		TIMERX6->Timer6LoadCount = loadcount;
		break;
	default:
		ret = false;
	}
	return ret;
}

bool timer_init(timer_init_config_t const *const timer_init_config)
{
	timer_id_t timer_id = timer_init_config->timer_id;
	timer_mode_t timer_mode = timer_init_config->timer_mode;
	bool int_mask = timer_init_config->int_mask;
	uint32_t loadcount = timer_init_config->loadcount;

	//1.Disable the timer
	timer_disable(timer_id);

	//2.set the timer mode
	timer_set_mode(timer_id, timer_mode);

	//3.Set the interrupt mask
	timer_set_int_mask(timer_id, int_mask);

	//4.set the LoadCount
	timer_set_loadcount(timer_id, loadcount);

	return true;
}
