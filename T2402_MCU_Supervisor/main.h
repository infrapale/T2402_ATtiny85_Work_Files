#ifndef __MAIN_H__
#define __MAIN_H__

#define  LED            (4u)
#define  LED_GREEN      (3u)
#define  TEST_PIN_1     (1u)
#define  I2C_ADDR       13
#define  I2C_RX_BUFF_SIZE   (16)
#define  I2C_TX_BUFF_SIZE   (16)
#define  I2C_REG_SIZE       (64)


#define DEFAULT_WDT_TIMEOUT  4000 
#define DEFAULT_OFF_TIMEOUT  30000 
#define DEFAULT_ 


typedef enum
{
  REG_ADDR_WDT_TIMEOUT      = 0x00,
  REG_ADDR_OFF_TIME         = 0x04,
  REG_ADDR_CLEAR_WATCHDOG   = 0x08,
  REG_ADDR_SWITCH_OFF       = 0x09,
  REG_ADDR_EEPROM_ADDR      = 0x0A,
  REG_ADDR_EEPROM_FUNC      = 0x0C,
  REG_ADDR_EEPROM_STATUS    = 0x0D,
  REG_ADDR_EEPROM_READ      = 0x10,
  REG_ADDR_EEPROM_WRITE     = 0x20,
  REG_ADDR_3 = 0x03,
  REG_ADDR_4 = 0x04,
  REG_ADDR_5 = 0x05,
} reg_addr_et;



typedef struct
{
    uint32_t  wd_timeout_ms;
    uint32_t  next_wd_reset_ms;
    uint32_t  off_timeout_ms;
    uint32_t  next_off_ms;   
    uint16_t  eeprom_addr;
    uint8_t   state;
    uint8_t   prev_state;
    uint8_t   sleep_state;
    uint8_t   reg_position;
    bool      new_msg;
    reg_addr_et reg_addr;
} cntrl_st;


extern cntrl_st cntrl;

extern uint8_t i2c_reg[I2C_REG_SIZE];

typedef enum 
{
  MASTER_TO_SLAVE = 0,
  SLAVE_TO_MASTER 
} master_slave_direction_et;

#endif