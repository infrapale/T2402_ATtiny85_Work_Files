#ifndef __MAIN_H__
#define __MAIN_H__

#define  PIN_PWR_OFF_0      (3u)
#define  PIN_CLR_EDOG       (4u)
#define  PIN_EXT_RESET      (1u)


#define  I2C_ADDR       13
#define  I2C_RX_BUFF_SIZE   (16)
#define  I2C_TX_BUFF_SIZE   (16)
#define  I2C_REG_SIZE       (64)


#define DEFAULT_WDT_TIMEOUT  4000
#define DEFAULT_RESET_TIME   1000
#define FORCED_WDT_TIMEOUT   60000
#define DEFAULT_OFF_TIMEOUT  30000 
#define DEFAULT_ 
// #define DEFAULT_WD_WINDOW_LOW  100
// #define DEFAULT_WD_WINDOW_HIGH 10000


#define OPTION_COMBINED_CONTROL 

#include "reg.h"

typedef enum
{
    SLEEP_STATE_RUNNING = 0,
    SLEEP_STATE_CLOSING,
    SLEEP_STATE_SLEEPING,
    SLEEP_STATE_WAKEUP
} sleep_state_et;

typedef struct
{
    //uint32_t  wd_timeout_ms;
    //uint32_t  wd_interval_ms;
    //uint32_t  wd_next_reset_ms;
    //uint32_t  wd_next_forced_reset_ms;
    uint32_t  sleep_time_ms;
    sleep_state_et sleep_state;
    uint8_t   reg_position;
    uint8_t   read_pos;
    bool      new_msg;
    bool      wd_is_active;
} cntrl_st;


extern cntrl_st cntrl;

extern uint8_t i2c_reg[I2C_REG_SIZE];

#endif