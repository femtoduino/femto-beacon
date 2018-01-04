
#include "asf.h"
#include <port.h>
#include <delay.h>
#include "sam0/utils/cmsis/samd21/include/pio/samd21e18a.h"

// See http://jjmilburn.github.io/2014/09/18/Atmel-SAMD20-EclipseCDT/

#define MY_LED    PIN_PA28

void configure_port_pins(void);
void configure_port_pins(void)
{
  struct port_config config_port_pin;

  port_get_config_defaults(&config_port_pin);
  config_port_pin.direction = PORT_PIN_DIR_OUTPUT;

  port_pin_set_config(MY_LED, &config_port_pin);
}

int main(void)
{
  system_init();
  delay_init();

  configure_port_pins();

  bool isRunning = true;
  bool pin_state = false;

  do {
      pin_state = !pin_state;
      port_pin_set_output_level(MY_LED, pin_state);
    delay_ms(500);
  } while (isRunning == true);
}
