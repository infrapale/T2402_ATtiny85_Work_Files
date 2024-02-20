#ifndef __MAIN_H__
#define __MAIN_H__

#define  TEST_PIN_GREEN   (3u)
#define  TEST_PIN_YELLOW  (4u)
#define  TEST_PIN_ORANGE  (1u)
#define  I2C_ADDR       13
#define  I2C_RX_BUFF_SIZE   (16)
#define  I2C_TX_BUFF_SIZE   (16)
#define  I2C_REG_SIZE       (64)


#define DEFAULT_WDT_TIMEOUT  50 
#define DEFAULT_OFF_TIMEOUT  30000 
#define DEFAULT_ 

#include "reg.h"


typedef struct
{
    //uint32_t  wd_timeout_ms;
    uint32_t  wd_interval_ms;
    uint32_t  wd_next_reset_ms;
    uint32_t  sleep_time_ms;
    uint8_t   sleep_state;
    uint8_t   reg_position;
    bool      new_msg;
} cntrl_st;


extern cntrl_st cntrl;

extern uint8_t i2c_reg[I2C_REG_SIZE];

#endif