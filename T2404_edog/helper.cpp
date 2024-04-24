#include "Arduino.h"
#include "main.h"


void blink_color_times(uint8_t pin, uint8_t n, uint16_t us)
{
  for (uint8_t i=0; i < n; i++)
  {
    digitalWrite(pin, HIGH);
    delayMicroseconds(us);
    digitalWrite(pin, LOW);
    delayMicroseconds(us);
  }
}

void goto_sleep(void)
{
    cntrl.sleep_state = SLEEP_STATE_CLOSING;
}