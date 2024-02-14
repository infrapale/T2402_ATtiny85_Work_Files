/******************************************************************************

*******************************************************************************


https://github.com/rambo/TinyWire
https://www.tastethecode.com/introducing-the-attiny-device-pcb-i2c-slave-devices

https://en.wikipedia.org/wiki/I%C2%B2C
https://learn.sparkfun.com/tutorials/sparkfun-qwiic-button-hookup-guide
https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2586-AVR-8-bit-Microcontroller-ATtiny25-ATtiny45-ATtiny85_Datasheet.pdf


******************************************************************************/

#include <TinyWireS.h>
#include <EEPROM.h>
#include "main.h"
#include "reg.h"
#include "ee_prom.h"



cntrl_st cntrl = 
{
  .wd_timeout_ms = DEFAULT_WDT_TIMEOUT,
  .next_wd_reset_ms = 0,
  .off_timeout_ms = DEFAULT_OFF_TIMEOUT,
  .next_off_ms = 0,
  .eeprom_addr = 0x0000,
  .state = 0,
  .prev_state = 0,
  .sleep_state = 0,
  .reg_position = 0,
  .new_msg = false,
  .reg_addr = 0,
};

//uint8_t i2c_reg[I2C_REG_SIZE];

uint8_t i2c_rx_buff[I2C_RX_BUFF_SIZE];

//char state = '0';
//char oldState = '0';
//unsigned long endMillis = 0;

void setup()
{
  pinMode(LED, OUTPUT );
  pinMode(LED_GREEN, OUTPUT );
  pinMode(TEST_PIN_1, OUTPUT );

  digitalWrite(LED_GREEN, LOW);
  digitalWrite(TEST_PIN_1, HIGH);
  
  TinyWireS.begin(I2C_ADDR);                
  TinyWireS.onReceive(receiveEvent);
  TinyWireS.onRequest(requestEvent);

  for (uint8_t i = 0; i < I2C_REG_SIZE; i++) 
  {
    i2c_reg[i] = i;
  }  
  // Read from EEPROM
  cntrl.wd_timeout_ms = ee_prom_rd_u32(EEPROM_ADDR_WD_TIMEOUT);
  cntrl.off_timeout_ms = ee_prom_rd_u32(EEPROM_ADDR_PWR_TIMEOUT);
  reg_wr_u32(REG_ADDR_WDT_TIMEOUT, cntrl.wd_timeout_ms);
  reg_wr_u32(REG_ADDR_SWITCH_OFF, cntrl.off_timeout_ms);



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

void sleep_statemachine(void)
{
  switch(cntrl.sleep_state)
  {
    case 0:  //idle
      break;
    case 1:
      break;
  }
}


void loop()
{
  TinyWireS_stop_check();

  if (cntrl.new_msg)
  {
    reg_action_on_receive( cntrl.reg_position );
    cntrl.new_msg = false;
  }

  reg_time_machine();
}


void receiveEvent(int howMany)
{
    digitalWrite(LED, HIGH);

    digitalWrite(TEST_PIN_1, HIGH);
    
    if (howMany < 1) return;
    
    if (howMany > I2C_RX_BUFF_SIZE) return;
    
    cntrl.reg_position = TinyWireS.receive();
    howMany--;
    if (!howMany)
    {
        // This write was only to set the buffer for next read
        //digitalWrite(TEST_PIN_1, LOW);
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

    //digitalWrite(TEST_PIN_1, LOW);
    cntrl.new_msg = true;
   //state = TinyWireS.receive();
}


void requestEvent()
{  
    //cntrl.reg_position = REG_ADDR_SET_WDT_TIMEOUT;
    //digitalWrite(LED_GREEN, HIGH);
    uint8_t bytes = reg_get_msg_len(cntrl.reg_position, SLAVE_TO_MASTER);
    for(uint8_t i = 0; i < bytes; i++)
    {
      TinyWireS.send(i2c_reg[cntrl.reg_position + i]);
    }
    //    

    //blink_n(bytes,4000);

}
 