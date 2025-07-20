#ifndef __MODULE_IR_H__
#define __MODULE_IR_H__
#include "stdint.h"
typedef struct module_ir
{
    uint8_t ir0;
    uint8_t ir_data[8]; // Buffer to hold IR data
} module_ir_t;

module_ir_t* ir_init();
void deal_IRdata();
void set_adjust_mode();
void ir_done(module_ir_t *m_ir);
#endif