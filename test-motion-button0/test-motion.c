
#include <contiki.h>
#include <stdio.h>
#include <lib/sensors.h>
#include "dev/common-clicks.h"
#include "dev/leds.h"
#include "sys/clock.h"
#include "button-sensor.h"

#define SENSOR1 &button_sensor
#define SENSOR2 &button_sensor2
/*---------------------------------------------------------------------------*/
PROCESS(test_motion, "Motion click test");
AUTOSTART_PROCESSES(&test_motion);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(test_motion, ev, data)
{
  PROCESS_EXITHANDLER(goto exit;)
  PROCESS_BEGIN();
  static int i;
 // static struct etimer timer;
  SENSORS_ACTIVATE(motion_sensor);
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL((ev == sensors_event));
    if (data == &motion_sensor) {
      printf("Motion event received\n");

      leds_on(LED1);
      /* Delay for 500ms */
      for(i=0;i<=500;++i) {
      clock_delay_usec(1000);
      }
      leds_off(LED1);

    }
    else if(data == SENSOR2)
    {
        leds_toggle(LED2);
    }
  }
  exit:
  SENSORS_DEACTIVATE(motion_sensor);
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
