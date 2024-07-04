#include "Arduino.h"
#include <TinyWireS.h>
#include <EEPROM.h>
#include "main.h"
#include "ee_prom.h"
#include "helper.h"


void ee_prom_wr_u32(uint16_t addr, uint32_t u32)
{
  EEPROM.update(addr++, (uint8_t)(u32 >> 24) & 0x00FF );
  EEPROM.update(addr++, (uint8_t)(u32 >> 16) & 0x00FF );
  EEPROM.update(addr++, (uint8_t)(u32 >> 8) & 0x00FF );
  EEPROM.update(addr,   (uint8_t)(u32 >> 0) & 0x00FF );
}

uint32_t ee_prom_rd_u32(uint16_t addr)
{
  uint32_t u32 = 0;
  uint8_t  u8;
  u8 = EEPROM.read(addr++);
  u32 |= (uint32_t)u8 << 24;
  u8 = EEPROM.read(addr++);
  u32 |= (uint32_t)u8 << 16;
  u8 = EEPROM.read(addr++);
  u32 |= (uint32_t)u8 << 8;
  u8 = EEPROM.read(addr++);
  u32 |= (uint32_t)u8 << 0;
  return u32;
}

void ee_prom_read_array(uint16_t addr, uint8_t *arr, uint8_t nbr)
{
  for (uint8_t i = 0; i < nbr; i++)
  {
    EEPROM.get(addr++,arr[i]);
  }

}

void ee_prom_write_array(uint16_t addr, uint8_t *arr, uint8_t nbr)
{
  //blink_color_times(PIN_EXT_RESET, nbr, 10);
  for (uint8_t i = 0; i < nbr; i++)
  {
    EEPROM.update(addr++, arr[i]);
  }
}

