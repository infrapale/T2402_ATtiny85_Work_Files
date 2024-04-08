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
#include "tinysnore.h"
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
  .read_pos = 0,
  .new_msg = false,
};

//uint8_t i2c_reg[I2C_REG_SIZE];

uint8_t i2c_rx_buff[I2C_RX_BUFF_SIZE];
uint32_t blink_time_ms;

void connect_io(void)
{
  pinMode(TEST_PIN_GREEN, OUTPUT );
  pinMode(TEST_PIN_YELLOW, OUTPUT );
  pinMode(TEST_PIN_ORANGE, OUTPUT );

  digitalWrite(TEST_PIN_GREEN, LOW);
  digitalWrite(TEST_PIN_YELLOW, LOW);
  digitalWrite(TEST_PIN_ORANGE, LOW);

}

void disconnect_io(void)
{
  pinMode(TEST_PIN_GREEN, INPUT );
  pinMode(TEST_PIN_YELLOW, INPUT );
  pinMode(TEST_PIN_ORANGE, INPUT );
  // pinMode(2U,INPUT);
  // pinMode(0U,INPUT);
}


void setup()
{
  connect_io();
  
  reg_initialize();

  uint32_t n = ee_prom_rd_u32(0x0010);
  if (n>8) n = 1;
  blink_color_times(TEST_PIN_YELLOW, n, 10);
  n++;
  ee_prom_wr_u32(0x0010, n);

  TinyWireS.begin(I2C_ADDR);                
  TinyWireS.onReceive(receiveEvent);
  TinyWireS.onRequest(requestEvent);
  blink_time_ms = millis() + 100;
}

void sleep_time_machine(void)
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
  if (cntrl.sleep_state > 0)
  {
    digitalWrite(TEST_PIN_GREEN, LOW);
    disconnect_io();
    snore(1000); 
    cntrl.sleep_state = 0;
    connect_io();
    digitalWrite(TEST_PIN_GREEN, HIGH);

  }
  // if(  millis() > blink_time_ms)
  // {
  //   blink_time_ms = millis() + 100;
  //   digitalWrite(TEST_PIN_GREEN, HIGH);
  //   delayMicroseconds(2);
  //   digitalWrite(TEST_PIN_GREEN, LOW);
  // }
}


void receiveEvent(int howMany)
{

    //digitalWrite(TEST_PIN_GREEN, HIGH);
    
    if (howMany < 1) return;
    
    if (howMany > I2C_RX_BUFF_SIZE) return;
    
    cntrl.reg_position = TinyWireS.read();
    blink_color_times(TEST_PIN_YELLOW, cntrl.reg_position, 1);
    cntrl.new_msg = true;
    howMany--;
    if (!howMany)
    {
        // This write was only to set the buffer for next read
        return;
    }
    uint8_t offs = 0;
    while(howMany--)
    {
        i2c_reg[cntrl.reg_position+offs] = TinyWireS.read();
        offs++;
        if ((cntrl.reg_position + offs) >= I2C_REG_SIZE)
        {
            offs = 0;
        }
    }

    //digitalWrite(TEST_PIN_GREEN, LOW);

   //state = TinyWireS.receive();
}


void requestEvent()
{  
    uint8_t bytes = reg_get_msg_len(cntrl.read_pos, SLAVE_TO_MASTER);
    for(uint8_t i = 0; i < bytes; i++)
    {
      TinyWireS.write(i2c_reg[cntrl.read_pos + i]);
      //TinyWireS.write(bytes + i);
    }
}
 