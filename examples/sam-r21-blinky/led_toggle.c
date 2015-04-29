
#include "asf.h"
#include "conf_example.h"
#include <port.h>
#include <ioport.h>
#include <delay.h>
#include "sam0/utils/cmsis/samd21/include/pio/samd21e18a.h"

// See http://jjmilburn.github.io/2014/09/18/Atmel-SAMD20-EclipseCDT/

#define MY_LED    PIN_PA08

int main(void)
{
  system_init();
  delay_init();
  ioport_init();
  ioport_set_pin_dir(MY_LED, IOPORT_DIR_OUTPUT);

  bool isRunning = true;

  do {

    ioport_toggle_pin_level(MY_LED);
    delay_ms(500);
  } while (isRunning == true);
}
