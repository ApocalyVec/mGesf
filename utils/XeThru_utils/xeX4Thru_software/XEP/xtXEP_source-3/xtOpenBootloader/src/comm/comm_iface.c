/**
 * @file
 *
 * @brief Communication interface abstraction layer functions
 *
 */ 
 #include <asf.h>
 #include "comm_iface.h"
 #include <comm/cdc.h>

static uint8_t fifo_buffer_TX[FIFO_BUFFER_LENGTH];
static uint8_t fifo_buffer_RX[FIFO_BUFFER_LENGTH];
static fifo_desc_t fifo[2];
static ifctype_t s_ifctype=IFACE_NONE;


void InitCommIface(ifctype_t type){
	s_ifctype=type;
	fifo_init(&fifo[IFACE_OUT], fifo_buffer_TX, FIFO_BUFFER_LENGTH);
	fifo_init(&fifo[IFACE_IN], fifo_buffer_RX, FIFO_BUFFER_LENGTH);
	switch(s_ifctype){
		case IFACE_USB:		cdc_init(); break;
		case IFACE_USART:	break;
		default:	
		case IFACE_NONE: break;
	}
}

void CommIface_put(ifcdir_t dir, uint8_t b)
{
	uint8_t status=FIFO_OK;
	irqflags_t flags = cpu_irq_save();
	status =  fifo_push_uint8(&fifo[dir], b);
	Assert(status==FIFO_OK);
	cpu_irq_restore(flags);
}

uint8_t CommIface_get(ifcdir_t dir)
{
	uint8_t ret = 0;
	irqflags_t flags = cpu_irq_save();
	uint8_t status=fifo_pull_uint8(&fifo[dir], &ret);
	Assert(status==FIFO_OK);
	cpu_irq_restore(flags);
	return ret;
}

bool CommIface_is_empty(ifcdir_t dir)
{
	bool is_empty;
	irqflags_t flags = cpu_irq_save();
	is_empty = fifo_is_empty(&fifo[dir]);
	cpu_irq_restore(flags);
	return is_empty;
}

void CommIface_flush(ifcdir_t iface)
{
	irqflags_t flags = cpu_irq_save();
	switch(s_ifctype){
		case IFACE_USB:		cdc_flush(); break;
		case IFACE_USART:	break;
		default:
		case IFACE_NONE: break;
	}
	cpu_irq_restore(flags);
}

void CommIface_Clear(ifcdir_t dir){
	irqflags_t flags = cpu_irq_save();
	fifo_flush(&fifo[dir]);
	cpu_irq_restore(flags);
}