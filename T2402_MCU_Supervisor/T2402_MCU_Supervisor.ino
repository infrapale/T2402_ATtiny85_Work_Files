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
#include "helper.h"



cntrl_st cntrl = 
{
  .wd_interval_ms   = DEFAULT_WDT_TIMEOUT,
  .wd_next_reset_ms = DEFAULT_WDT_TIMEOUT,
  .sleep_time_ms    = DEFAULT_OFF_TIMEOUT,
  .sleep_state = 0,
  .reg_position = 0,
  .new_msg = false,
};

//uint8_t i2c_reg[I2C_REG_SIZE];

uint8_t i2c_rx_buff[I2C_RX_BUFF_SIZE];


void setup()
{
  pinMode(TEST_PIN_GREEN, OUTPUT );
  pinMode(TEST_PIN_YELLOW, OUTPUT );
  pinMode(TEST_PIN_ORANGE, OUTPUT );

  digitalWrite(TEST_PIN_GREEN, HIGH);
  digitalWrite(TEST_PIN_YELLOW, HIGH);
  digitalWrite(TEST_PIN_ORANGE, HIGH);
  
  reg_initialize();

  TinyWireS.begin(I2C_ADDR);                
  TinyWireS.onReceive(receiveEvent);
  TinyWireS.onRequest(requestEvent);

  // uint32_t u32 = 0;
  // uint32_t u32rd = 0;

  // for (uint8_t i=0; i < 8; i++)
  // {
  //   u32++;
  //   ee_prom_wr_u32(0x40,u32);
  //   blink_color_times(TEST_PIN_YELLOW,(uint8_t)u32, 1);
  //   u32rd = ee_prom_rd_u32(0x40);
  //   blink_color_times(TEST_PIN_ORANGE,(uint8_t)u32rd, 1);
  // }

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

    //digitalWrite(TEST_PIN_GREEN, HIGH);
    
    if (howMany < 1) return;
    
    if (howMany > I2C_RX_BUFF_SIZE) return;
    
    cntrl.reg_position = TinyWireS.receive();
    cntrl.new_msg = true;
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

    digitalWrite(TEST_PIN_GREEN, LOW);

   //state = TinyWireS.receive();
}


void requestEvent()
{  
    uint8_t bytes = reg_get_msg_len(cntrl.reg_position, SLAVE_TO_MASTER);
    for(uint8_t i = 0; i < bytes; i++)
    {
      TinyWireS.send(i2c_reg[cntrl.reg_position + i]);
    }
}
 