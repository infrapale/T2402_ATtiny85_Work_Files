#ifndef __EE_PROM_H__
#define __EE_PROM_H__


typedef enum
{
  EEPROM_FUNC_READ        = 0x01,
  EEPROM_FUNC_WRITE       = 0x02,
}  eeprom_func_et;

typedef enum
{
  EEPROM_STATUS_RD_READY    = 0x01,
  EEPROM_STATUS_WR_READY    = 0x02,
}  eeprom_status_et;

typedef enum
{
  EEPROM_ADDR_USER_START  = 0x0000,
  EEPROM_ADDR_USER_END    = 0x017F,
  EEPROM_ADDR_WD_INTERVAL = 0x0180,
  EEPROM_ADDR_SLEEP_TIME  = 0x0184,
  EEPROM_ADDR_LAST        = 0x01FF,
  EEPROM_ADDR_2           = 0x018C,

} eeprom_addr_et;

void ee_prom_wr_u32(uint16_t addr, uint32_t u32);

uint32_t ee_prom_rd_u32(uint16_t addr);

void ee_prom_read_array(uint16_t addr, uint8_t *arr, uint8_t nbr);

void ee_prom_write_array(uint16_t addr, uint8_t *arr, uint8_t nbr);

#endif