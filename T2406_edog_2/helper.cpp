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

void power_off(void)
{
    digitalWrite(PIN_PWR_OFF_0, HIGH);
    #ifdef OPTION_COMBINED_CONTROL
      // digitalWrite(PIN_PWR_OFF_1, HIGH);
      digitalWrite(PIN_EXT_RESET, HIGH);
    #endif
}

void power_on(void)
{
    digitalWrite(PIN_PWR_OFF_0, LOW);
    #ifdef OPTION_COMBINED_CONTROL
      // digitalWrite(PIN_PWR_OFF_1, LOW);
      digitalWrite(PIN_EXT_RESET, LOW);
    #endif
}
