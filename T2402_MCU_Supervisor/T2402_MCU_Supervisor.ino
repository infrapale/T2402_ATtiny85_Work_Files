/******************************************************************************

*******************************************************************************


https://github.com/rambo/TinyWire
https://www.tastethecode.com/introducing-the-attiny-device-pcb-i2c-slave-devices

https://en.wikipedia.org/wiki/I%C2%B2C
https://learn.sparkfun.com/tutorials/sparkfun-qwiic-button-hookup-guide
https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2586-AVR-8-bit-Microcontroller-ATtiny25-ATtiny45-ATtiny85_Datasheet.pdf


******************************************************************************/

#include <TinyWireS.h>
#define  LED            (4u)
#define  LED_GREEN      (3u)
#define  I2C_ADDR       13
#define  I2C_RX_BUFF_SIZE   (16)
#define  I2C_TX_BUFF_SIZE   (16)
#define  I2C_REG_SIZE       (64)


#define DEFAULT_WDT_TIMEOUT  5000 
#define DEFAULT_OFF_TIMEOUT  30000 
#define DEFAULT_ 


typedef enum
{
  REG_ADDR_SET_WDT_TIMEOUT  = 0x00,
  REG_ADDR_SET_OFF_TIME     = 0x04,
  REG_ADDR_CLEAR_WATCHDOG   = 0x08,
  REG_ADDR_SWITCH_OFF       = 0x09,
  REG_ADDR_EEPROM_ADDR      = 0x0A,
  REG_ADDR_EEPROM_READ      = 0x10,
  REG_ADDR_EEPROM_WRITE     = 0x20,
  REG_ADDR_3 = 0x03,
  REG_ADDR_4 = 0x04,
  REG_ADDR_5 = 0x05,
} reg_addr_et;



typedef struct
{
    uint32_t  next_wd_reset_ms;
    uint32_t  next_off_ms;   
    uint16_t  eeprom_addr;
    uint8_t   state;
    uint8_t   prev_state;
    uint8_t   reg_position;
    bool      new_msg;
    reg_addr_et reg_addr;
} cntrl_st;


cntrl_st cntrl = 
{
  .next_wd_reset_ms = 0,
  .next_off_ms = 0,
  .eeprom_addr = 0x0000,
  .state = 0,
  .prev_state = 0,
  .reg_position = 0,
  .new_msg = false,
  .reg_addr = 0,
};

uint8_t i2c_reg[I2C_REG_SIZE];

uint8_t i2c_rx_buff[I2C_RX_BUFF_SIZE];

char state = '0';
char oldState = '0';
unsigned long endMillis = 0;

void setup()
{
  pinMode(LED, OUTPUT );
  pinMode(LED_GREEN, OUTPUT );
  digitalWrite(LED_GREEN, HIGH);
  TinyWireS.begin(I2C_ADDR);                
  TinyWireS.onReceive(receiveEvent);
  TinyWireS.onRequest(requestEvent);
}

void loop()
{
  TinyWireS_stop_check();
  
  if (cntrl.new_msg)
  {    
    switch(cntrl.reg_position)
    {
      case REG_ADDR_SET_WDT_TIMEOUT:
        break;
      case REG_ADDR_SET_OFF_TIME:
        break;
      case REG_ADDR_CLEAR_WATCHDOG:
        break;
      case REG_ADDR_SWITCH_OFF:
        digitalWrite(LED, LOW);
        break;
      case REG_ADDR_EEPROM_ADDR:
        break;
      case REG_ADDR_EEPROM_READ:
        break;
      case REG_ADDR_EEPROM_WRITE:
        break;
      default:
        digitalWrite(LED, LOW);
        break;
    }
    cntrl.new_msg = false;
  }
}


uint8_t get_msg_len(reg_addr_et reg_addr)
{
    uint8_t nbr_bytes = 0;
    switch(reg_addr)
    {
      case REG_ADDR_SET_WDT_TIMEOUT:
        nbr_bytes = 4;
        break;
      case REG_ADDR_SET_OFF_TIME:
        nbr_bytes = 4;
        break;
      case REG_ADDR_CLEAR_WATCHDOG:
        nbr_bytes = 4;
        break;
      case REG_ADDR_SWITCH_OFF:
        nbr_bytes = 4;
        break;
      case REG_ADDR_EEPROM_ADDR:
        nbr_bytes = 4;
        break;
      case REG_ADDR_EEPROM_READ:
        nbr_bytes = 4;
        break;
      case REG_ADDR_EEPROM_WRITE:
        nbr_bytes = 4;
        break;
      default:
        nbr_bytes = 4;
        break;
    }
  return nbr_bytes;
}

void receiveEvent(int howMany)
{
    digitalWrite(LED, HIGH);
    if (howMany < 1)
    {
        // Sanity-check
        return;
    }
    if (howMany > I2C_RX_BUFF_SIZE)
    {
        // Also insane number
        return;
    }

    cntrl.reg_position = TinyWireS.receive();
    howMany--;
    if (!howMany)
    {
        // This write was only to set the buffer for next read
        return;
    }
    uint8_t offs = 0;
    while(howMany--)
    {
        i2c_reg[cntrl.reg_position+offs] = TinyWireS.receive();
        offs++;
        if (cntrl.reg_position + offs >= I2C_REG_SIZE)
        {
            offs = 0;
        }
    }
    cntrl.new_msg = true;
   


  //state = TinyWireS.receive();
}


void requestEvent()
{  
    uint8_t bytes = get_msg_len(cntrl.reg_position);
    digitalWrite(LED_GREEN, LOW);   
    for(uint8_t i = 0; i < bytes; i++)
    {
      TinyWireS.send(i2c_reg[cntrl.reg_position + i]);
    }

    // Increment the reg position on each read, and loop back to zero
    cntrl.reg_position++;
    if (cntrl.reg_position >= I2C_REG_SIZE)
    {
        cntrl.reg_position = 0;
    }
}
