#ifndef __REG_H__
#define __REG_H__

#include "main.h"

typedef enum
{
  REG_ADDR_WD_INTERVAL      = 0x00,
  REG_ADDR_SLEEP_TIME       = 0x04,
  REG_ADDR_CLEAR_WATCHDOG   = 0x08,
  REG_ADDR_SWITCH_OFF       = 0x09,
  REG_ADDR_EEPROM_ADDR      = 0x0A,
  REG_ADDR_EEPROM_LOAD      = 0x0C,
  REG_ADDR_EEPROM_SAVE      = 0x0D,
  REG_ADDR_POWER_OFF_0      = 0x0E,
  REG_ADDR_EXT_RESET        = 0x0F,
  REG_ADDR_EEPROM_READ      = 0x10,
  REG_ADDR_EEPROM_WRITE     = 0x20,
  REG_ADDR_4 = 0x0F,
  REG_ADDR_5 = 0x05,
} reg_addr_et;


typedef enum 
{
  MASTER_TO_SLAVE = 0,
  SLAVE_TO_MASTER 
} master_slave_direction_et;


typedef struct
{
    uint8_t   state;
    uint32_t  timeout_ms;
    uint32_t  interval_ms;
    uint32_t  next_reset_ms;
    uint32_t  next_forced_reset_ms;
    bool      is_active;
} wd_st;


void reg_initialize(void);

void reg_warm_setup(void);

uint32_t reg_rd_u32(uint8_t reg_addr);

void reg_wr_u32(uint8_t reg_addr, uint32_t u32);

uint8_t reg_get_msg_len(reg_addr_et reg_addr, master_slave_direction_et master_slave);

void reg_action_on_receive(reg_addr_et reg_addr);

void reg_wd_state_machine(void);

void reg_time_machine(void);

#endif