#include "Arduino.h"
#include "main.h"
#include "reg.h"
#include "ee_prom.h"

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
    uint32_t wd_timeout_ms;
    uint32_t wd_interval_ms;

} reg_st;




uint8_t i2c_reg[I2C_REG_SIZE];

reg_st reg;

uint32_t reg_rd_u32(uint8_t reg_addr)
{
  uint32_t u32 = 0;
  u32 |= i2c_reg[reg_addr] << 24;
  u32 |= i2c_reg[reg_addr] << 16;
  u32 |= i2c_reg[reg_addr] << 8;
  u32 |= i2c_reg[reg_addr] << 0;
  return u32;
}

void reg_wr_u32(uint8_t reg_addr, uint32_t u32)
{

  i2c_reg[reg_addr++] = (uint8_t)((u32 >> 24) & 0x000000FF);
  i2c_reg[reg_addr++] = (uint8_t)((u32 >> 16) & 0x000000FF);
  i2c_reg[reg_addr++] = (uint8_t)((u32 >> 8) & 0x000000FF);
  i2c_reg[reg_addr] =   (uint8_t)((u32 >> 0) & 0x000000FF);
}



uint8_t reg_get_msg_len(reg_addr_et reg_addr, master_slave_direction_et master_slave)
{
  uint8_t m2s_bytes = 0;
  uint8_t s2m_bytes = 0;
  switch(reg_addr)
  {
    case REG_ADDR_WDT_TIMEOUT:
      m2s_bytes = 4;
      s2m_bytes = 4;
      break;
    case REG_ADDR_OFF_TIME:
      m2s_bytes = 4;
      s2m_bytes = 4;
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
      s2m_bytes = 2;
      break;
    case REG_ADDR_EEPROM_FUNC:
      m2s_bytes = 1;
      s2m_bytes = 0;
      break;
    case REG_ADDR_EEPROM_STATUS:
      m2s_bytes = 1;
      s2m_bytes = 1;
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
          reg.timeout_ms = millis() + 100;
          reg.state = 10;
          break;
        case REG_ACTION_EEPROM_RD:
          reg.timeout_ms = millis() + 100;
          reg.state = 20;
          break;
        case REG_ACTION_SWITCH_OFF:
          reg.timeout_ms = millis() + 1000;
          reg.state = 30;
          break;
      }
      if (millis() > reg.wd_timeout_ms)
      {
          digitalWrite(LED_GREEN, HIGH);
          reg.timeout_ms = millis() + 100;
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
          digitalWrite(TEST_PIN_1, LOW);
          reg.state = 0;
          reg.action = REG_ACTION_UNDEF;
      } 

      break;
    case 40:  //
      if (millis() > reg.timeout_ms)
      {
          reg.wd_timeout_ms = millis() +reg.wd_interval_ms;
          digitalWrite(LED_GREEN, LOW);
          reg.state = 0;
      } 
      break;
    case 50:  //
      break;
    case 60:  //
      break;
    case 70:  //
      break;

  }

}

void reg_action_on_receive(reg_addr_et reg_addr)
{
    uint32_t u32;
    switch(cntrl.reg_position)
    {
      case REG_ADDR_WDT_TIMEOUT:
        u32 = reg_rd_u32(cntrl.reg_position);
        ee_prom_wr_u32( EEPROM_ADDR_WD_TIMEOUT, u32);
        reg.wd_interval_ms =  4000;   // u32;
        reg.action = REG_ACTION_EEPROM_WR;
        break;
      case REG_ADDR_OFF_TIME:
        u32 = reg_rd_u32(cntrl.reg_position);
        ee_prom_wr_u32( EEPROM_ADDR_WD_TIMEOUT, u32);
        reg.action = REG_ACTION_EEPROM_WR;
        break;
      case REG_ADDR_CLEAR_WATCHDOG:
        digitalWrite(TEST_PIN_1, HIGH);
        cntrl.next_wd_reset_ms = millis() +  reg.wd_interval_ms;  //  cntrl.next_wd_reset_ms;
        digitalWrite(TEST_PIN_1, LOW);
        break;
      case REG_ADDR_SWITCH_OFF:
        digitalWrite(LED, LOW);
        break;
      case REG_ADDR_EEPROM_ADDR:
        reg.eeprom_addr = reg_rd_u32(cntrl.reg_position);
        break;
      case REG_ADDR_EEPROM_FUNC:
        uint8_t eeprom_func =  i2c_reg[cntrl.reg_position];
        if (reg.eeprom_addr <= EEPROM_ADDR_LAST)
        switch(eeprom_func)
        {
          case EEPROM_FUNC_READ:
            ee_prom_read_array( reg.eeprom_addr, &i2c_reg[REG_ADDR_EEPROM_READ], 8);
            reg.action = REG_ACTION_EEPROM_RD;
            break;
          case EEPROM_FUNC_WRITE:
            ee_prom_write_array( reg.eeprom_addr, &i2c_reg[REG_ADDR_EEPROM_WRITE], 8);
            reg.action = REG_ACTION_EEPROM_WR;
            break;
        }

        break;
      case REG_ADDR_EEPROM_STATUS:
        break;
      case REG_ADDR_EEPROM_READ:
        break;
      case REG_ADDR_EEPROM_WRITE:
        break;
      default:
        digitalWrite(LED, LOW);
        break;
    }  
  }
