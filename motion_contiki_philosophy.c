#include <stdio.h>

#include <contiki.h>

#include <sys/clock.h>

#include "dev/leds.h"

#include "letmecreate/click/motion.h"
#include "letmecreate/core/common.h"

#include "letmecreate/core/debug.h"

static process_event_t e;

PROCESS(main_process, "Main process");
AUTOSTART_PROCESSES(&main_process);

void motion_callback(uint8_t event)
{
    process_post(&main_process,e,NULL); //async to p1
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(main_process, ev, data)
{
    PROCESS_BEGIN();
    e = process_alloc_event();
    INIT_NETWORK_DEBUG();
    {
        PRINTF("=====Start=====\n");

        motion_click_enable(MIKROBUS_1);

        motion_click_attach_callback(MIKROBUS_1, motion_callback);
        while(1)
        {
            PROCESS_WAIT_EVENT(); //wait 4 events
            if (ev == e)
                leds_toggle(LED1);
        }

        motion_click_disable(MIKROBUS_1);
    }

    PROCESS_END();
}

/*---------------------------------------------------------------------------*/
