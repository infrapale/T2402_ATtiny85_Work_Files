#include "Arduino.h"
#include "main.h"
#include "reg.h"
#include "ee_prom.h"
#include "helper.h"


typedef enum
{
  REG_ACTION_UNDEF = 0,
  REG_ACTION_EEPROM_WR,
  REG_ACTION_EEPROM_RD,
  REG_ACTION_SWITCH_OFF ,
  REG_ACTION_3 ,
  REG_ACTION_4 
} reg_action_et;

typedef struct
{
    uint8_t state;
    reg_action_et action;
    uint32_t timeout_ms;
    uint16_t eeprom_addr;

} reg_st;


extern cntrl_st cntrl;

uint8_t i2c_reg[I2C_REG_SIZE];

reg_st reg;

void reg_initialize(void)
{
    // Read from EEPROM
  reg.state = 0;  
  cntrl.wd_interval_ms = ee_prom_rd_u32(EEPROM_ADDR_WD_INTERVAL);
  cntrl.sleep_time_ms  = ee_prom_rd_u32(EEPROM_ADDR_SLEEP_TIME);
  if (cntrl.wd_interval_ms > 60000 ) cntrl.wd_interval_ms = 10000;
  if (cntrl.sleep_time_ms > 60000) cntrl.sleep_time_ms = 10000;
  reg_wr_u32(REG_ADDR_WD_INTERVAL, cntrl.wd_interval_ms);
  reg_wr_u32(REG_ADDR_SLEEP_TIME, cntrl.sleep_time_ms);

}

uint32_t reg_rd_u32(uint8_t reg_addr)
{
  uint32_t u32 = 0;
  u32 |= i2c_reg[reg_addr++] << 24;
  u32 |= i2c_reg[reg_addr++] << 16;
  u32 |= i2c_reg[reg_addr++] << 8;
  u32 |= i2c_reg[reg_addr] << 0;
  return u32;
}

uint16_t reg_rd_u16(uint8_t reg_addr)
{
  uint32_t u16 = 0;
  u16 |= i2c_reg[reg_addr++] << 8;
  u16 |= i2c_reg[reg_addr] << 0;
  return u16;
}

void reg_wr_u32(uint8_t reg_addr, uint32_t u32)
{
  i2c_reg[reg_addr++] = (uint8_t)((u32 >> 24) & 0x000000FF);
  i2c_reg[reg_addr++] = (uint8_t)((u32 >> 16) & 0x000000FF);
  i2c_reg[reg_addr++] = (uint8_t)((u32 >> 8) & 0x000000FF);
  i2c_reg[reg_addr] =   (uint8_t)((u32 >> 0) & 0x000000FF);
}

void reg_wr_u16(uint8_t reg_addr, uint32_t u16)
{
  i2c_reg[reg_addr++] = (uint8_t)((u16 >> 8) & 0x000000FF);
  i2c_reg[reg_addr] =   (uint8_t)((u16 >> 0) & 0x000000FF);
}



uint8_t reg_get_msg_len(reg_addr_et reg_addr, master_slave_direction_et master_slave)
{
  uint8_t m2s_bytes = 0;
  uint8_t s2m_bytes = 0;
  switch(reg_addr)
  {
    case REG_ADDR_WD_INTERVAL:
      m2s_bytes = 4;
      s2m_bytes = 0;
      break;
    case REG_ADDR_SLEEP_TIME:
      m2s_bytes = 4;
      s2m_bytes = 0;
      break;
    case REG_ADDR_CLEAR_WATCHDOG:
      m2s_bytes = 1;
      s2m_bytes = 0;
      break;
    case REG_ADDR_SWITCH_OFF:
      m2s_bytes = 1;
      s2m_bytes = 0;
      break;
    case REG_ADDR_EEPROM_ADDR:
      m2s_bytes = 2;
      s2m_bytes = 0;
      break;
    case REG_ADDR_EEPROM_LOAD:
      m2s_bytes = 1;
      s2m_bytes = 0;
      break;
    case REG_ADDR_EEPROM_SAVE:
      m2s_bytes = 1;
      s2m_bytes = 0;
      break;
    case REG_ADDR_EEPROM_READ:
      m2s_bytes = 0;
      s2m_bytes = 8;
      break;
    case REG_ADDR_EEPROM_WRITE:
      m2s_bytes = 8;
      s2m_bytes = 0;
      break;
    default:
      m2s_bytes = 0;
      s2m_bytes = 0;
      break;
  }
  if (master_slave == MASTER_TO_SLAVE) return m2s_bytes;
  else return s2m_bytes;
}

void reg_time_machine(void)
{
  switch(reg.state)
  {
    case 0:  //idle
      switch(reg.action)
      {
        case REG_ACTION_EEPROM_WR:
          reg.timeout_ms = millis() + 10;
          reg.state = 10;
          break;
        case REG_ACTION_EEPROM_RD:
          reg.timeout_ms = millis() + 10;
          reg.state = 20;
          break;
        case REG_ACTION_SWITCH_OFF:
          reg.timeout_ms = millis() + 1000;
          reg.state = 30;
          break;

      }
      if (millis() >  cntrl.wd_next_reset_ms)
      {
          power_off();
          reg.timeout_ms = millis() + 10;
          reg.state = 40;
      }
      break;
    case 5:  //
      break;
    case 10:  // Write EEEPROM
      if (millis() > reg.timeout_ms)
      {
          reg.state = 0;
          reg.action = REG_ACTION_UNDEF;
      } 
      break;
    case 20:  // Read EEPROM
      if (millis() > reg.timeout_ms)
      {
          reg.state = 0;
          reg.action = REG_ACTION_UNDEF;
      } 
      break;
    case 30:  // Switch off
      if (millis() > reg.timeout_ms)
      {
          reg.state = 0;
          reg.action = REG_ACTION_UNDEF;
          cntrl.sleep_state = 1;
      } 

      break;
    case 40:  //
      if (millis() > reg.timeout_ms)
      {
          power_on();
          cntrl.wd_next_reset_ms = millis() + 200;  //cntrl.wd_interval_ms;
          reg.state = 0;
      } 
      break;
    case 50:  //
      break;
    case 60:  //
      break;
    default:  
      reg.state = 0;
      break;

  }
}

void reg_action_on_receive(reg_addr_et reg_addr)
{
    uint32_t u32;

    //blink_color_times(TEST_PIN_GREEN, reg_addr, 1);
    cntrl.read_pos = reg_addr;
    switch(reg_addr)
    {
      case REG_ADDR_WD_INTERVAL:
        u32 = reg_rd_u32(cntrl.reg_position);
        ee_prom_wr_u32( EEPROM_ADDR_WD_INTERVAL, u32);
        cntrl.wd_interval_ms =  u32;
        reg.action = REG_ACTION_EEPROM_WR;
        break;
      case REG_ADDR_SLEEP_TIME:
        u32 = reg_rd_u32(cntrl.reg_position);
        ee_prom_wr_u32( EEPROM_ADDR_SLEEP_TIME, u32);
        reg.action = REG_ACTION_EEPROM_WR;
        break;
      case REG_ADDR_CLEAR_WATCHDOG:
        cntrl.wd_next_reset_ms = millis() +  200;  // cntrl.wd_interval_ms; 
        break;
      case REG_ADDR_SWITCH_OFF:
        goto_sleep();
        break;
      case REG_ADDR_EEPROM_ADDR:
        reg.eeprom_addr = reg_rd_u16(cntrl.reg_position);
        break;
      case REG_ADDR_EEPROM_LOAD:
        ee_prom_read_array( reg.eeprom_addr, &i2c_reg[REG_ADDR_EEPROM_READ], 8);
        reg.action = REG_ACTION_EEPROM_RD;
        cntrl.read_pos = REG_ADDR_EEPROM_READ;
        break;
      case REG_ADDR_EEPROM_SAVE:
        ee_prom_write_array( reg.eeprom_addr, &i2c_reg[REG_ADDR_EEPROM_WRITE], 8);
        reg.action = REG_ACTION_EEPROM_WR;
        break;
      case REG_ADDR_POWER_OFF_1:
        if (i2c_reg[REG_ADDR_POWER_OFF_1] != 1)  digitalWrite(PIN_PWR_OFF_1, LOW);
        else  digitalWrite(PIN_PWR_OFF_1, HIGH);
        break;
      case REG_ADDR_EXT_RESET:
        if (i2c_reg[REG_ADDR_EXT_RESET] != 1)  digitalWrite(PIN_EXT_RESET, HIGH);
        else  digitalWrite(PIN_EXT_RESET, LOW);
        break;
      case REG_ADDR_EEPROM_READ:
        break;
      case REG_ADDR_EEPROM_WRITE:
        break;
      default:
        
        break;
    }  
  }
