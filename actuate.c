#include "../tism/tism-c/tism.h"
#include <assert.h>
#include <stdlib.h>
#include "pwm.h"
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>

/*
 * Accepts arguments for period in nanoseconds, pwmchip, channel id
 * If no arguments are provided, defaults to a period of 1ms (1000000ns), pwmchip path of PIN_12_PATH, and channel id of PIN_12_CHANNEL_ID
*/
int main(char** argv, int argc) {
    tism_borrowed_shared_memory_t shm;

    int period = 1000000; // default to 1ms period, or 1kHz frequency
    char* pwmchip_path = PIN_12_PATH; // default to the pwmchip that controls pin 12
    int channel_id = PIN_12_CHANNEL_ID; // default to the channel that corresponds to pin 12

    // needs error handling for invalid arguments
    for (int i = 1; i < argc; i++) {
        switch (i) {
        case 1:
            period = atoi(argv[i]);
            break;
        case 2:
            pwmchip_path = argv[i];
            break;
        case 3:
            channel_id = atoi(argv[i]);
            break;
        default:
            break;
        }
    }

    device_obj dev_obj = {pwmchip_path};
    device_obj *device = &dev_obj;

    initialize_pwm(device, channel_id, period);

    /*
     * Consumers don't initialize allocations with a value, and understand the
     * size of the allocation implicitly.
     */

    
    TISM_MBIND(tism_wait_and_open(&shm, "temp_name"));


    while (true) {  
        int duty_cycle;
        if (tism_borrowed_has_changed(&shm)) {
            TISM_MBIND(tism_borrowed_read(&shm, &duty_cycle));
            if (set_duty_cycle(device, channel_id, duty_cycle) != 0) {
                printf("Failed to set duty cycle\n");
            }
        }
        usleep(period / 2); // sleep for half the period to avoid busy waiting
    }
    
 

}

void cleanup(device_obj* device, int channel_id, tism_borrowed_shared_memory_t shm) {
    terminate_pwm(device, channel_id);
    TISM_MBIND(tism_borrowed_close(&shm));
}

