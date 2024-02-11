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
#define  TEST_PIN_1     (1u)
#define  I2C_ADDR       13
#define  I2C_RX_BUFF_SIZE   (16)
#define  I2C_TX_BUFF_SIZE   (16)
#define  I2C_REG_SIZE       (64)


#define DEFAULT_WDT_TIMEOUT  5000 
#define DEFAULT_OFF_TIMEOUT  30000 
#define DEFAULT_ 

typedef enum 
{
  MASTER_TO_SLAVE = 0,
  SLAVE_TO_MASTER 
} master_slave_direction_et;

typedef enum
{
  REG_ADDR_SET_WDT_TIMEOUT  = 0x00,
  REG_ADDR_SET_OFF_TIME     = 0x04,
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
  pinMode(TEST_PIN_1, OUTPUT );

  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(TEST_PIN_1, LOW);
  
  TinyWireS.begin(I2C_ADDR);                
  TinyWireS.onReceive(receiveEvent);
  TinyWireS.onRequest(requestEvent);

  for (uint8_t i = 0; i < I2C_REG_SIZE; i++) 
  {
    i2c_reg[i] = i;
  }  
}

void blink_n(uint8_t n, uint16_t ms)
{
  for (uint8_t i=0; i < n; i++)
  {
    digitalWrite(LED_GREEN, HIGH);
    delay(ms);
    digitalWrite(LED_GREEN, LOW);
    delay(ms);
  }

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
      case REG_ADDR_EEPROM_FUNC:
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
    cntrl.new_msg = false;



  }
}


uint8_t get_msg_len(reg_addr_et reg_addr, master_slave_direction_et master_slave)
{
  uint8_t m2s_bytes = 0;
  uint8_t s2m_bytes = 0;
  switch(reg_addr)
  {
    case REG_ADDR_SET_WDT_TIMEOUT:
      m2s_bytes = 4;
      s2m_bytes = 4;
      break;
    case REG_ADDR_SET_OFF_TIME:
      m2s_bytes = 4;
      s2m_bytes = 4;
      break;
    case REG_ADDR_CLEAR_WATCHDOG:
      m2s_bytes = 1;
      s2m_bytes = 1;
      break;
    case REG_ADDR_SWITCH_OFF:
      m2s_bytes = 1;
      s2m_bytes = 1;
      break;
    case REG_ADDR_EEPROM_ADDR:
      m2s_bytes = 4;
      s2m_bytes = 4;
      break;
    case REG_ADDR_EEPROM_FUNC:
      m2s_bytes = 1;
      s2m_bytes = 1;
      break;
    case REG_ADDR_EEPROM_STATUS:
      m2s_bytes = 1;
      s2m_bytes = 1;
      break;
    case REG_ADDR_EEPROM_READ:
      m2s_bytes = 1;
      s2m_bytes = 1;
      break;
    case REG_ADDR_EEPROM_WRITE:
      m2s_bytes = 1;
      s2m_bytes = 1;
      break;
    default:
      m2s_bytes = 0;
      s2m_bytes = 0;
      break;
  }
  if (master_slave == MASTER_TO_SLAVE) return m2s_bytes;
  else return s2m_bytes;
}

void receiveEvent(int howMany)
{
    digitalWrite(LED, HIGH);

    digitalWrite(TEST_PIN_1, HIGH);
    
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
        if ((cntrl.reg_position + offs) >= I2C_REG_SIZE)
        {
            offs = 0;
        }
    }

    digitalWrite(TEST_PIN_1, LOW);
    // for (uint8_t i = 0; i < I2C_REG_SIZE; i++) 
    // {
    //   i2c_reg[i] = (uint8_t) howMany;
    // }  
    cntrl.new_msg = true;
   


  //state = TinyWireS.receive();
}


void requestEvent()
{  
    //cntrl.reg_position = REG_ADDR_SET_WDT_TIMEOUT;
    uint8_t bytes = get_msg_len(cntrl.reg_position, SLAVE_TO_MASTER);
    digitalWrite(LED_GREEN, LOW);   
    for(uint8_t i = 0; i < bytes; i++)
    {
      TinyWireS.send(i2c_reg[cntrl.reg_position + i]);
    }

    //blink_n(bytes,4000);

}
