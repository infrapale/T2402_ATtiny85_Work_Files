#include "Arduino.h"
#include <TinyWireS.h>
#include <EEPROM.h>
#include "ee_prom.h"


void ee_prom_wr_u32(uint16_t addr, uint32_t u32)
{
  EEPROM.write(addr++, (u32 >> 24) & 0x00FF );
  EEPROM.write(addr++, (u32 >> 16) & 0x00FF );
  EEPROM.write(addr++, (u32 >> 8) & 0x00FF );
  EEPROM.write(addr,   (u32 >> 0) & 0x00FF );
}

uint32_t ee_prom_rd_u32(uint16_t addr)
{
  uint32_t u32 = 0;
  u32 |= EEPROM.read(addr) << 24;
  u32 |= EEPROM.read(addr) << 16;
  u32 |= EEPROM.read(addr) << 8;
  u32 |= EEPROM.read(addr) << 0;
  return u32;
}

void ee_prom_read_array(uint16_t addr, uint8_t *arr, uint8_t nbr)
{
  for (uint8_t i = 0; i < nbr; i++)
  {
    arr[i] = EEPROM.read(addr++);
  }
}

void ee_prom_write_array(uint16_t addr, uint8_t *arr, uint8_t nbr)
{
  for (uint8_t i = 0; i < nbr; i++)
  {
    EEPROM.write(addr++, arr[i]);
  }
}

