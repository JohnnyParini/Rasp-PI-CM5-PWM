#include "pwm.h"
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>

static device_obj *global_device = NULL;

void handler(int signum) {
    if (global_device != NULL) {
        set_duty_cycle(global_device, PIN_12_CHANNEL_ID, 0);
        terminate_pwm(global_device, PIN_12_CHANNEL_ID);
    }
    _exit(0);
}

/*This code sends a PWM signal to an actuator at a duty cycle of 10% */
int main(int argc, char** argv){


    int const period = 1000000;
    int const test_cycle = 100000;

    device_obj dev_obj = { PIN_12_PATH };
    device_obj *device = &dev_obj;
    global_device = device;

    signal(SIGINT, handler);
    signal(SIGTERM, handler);

    initialize_pwm(device, PIN_12_CHANNEL_ID, period);
    
    set_duty_cycle(device, PIN_12_CHANNEL_ID, test_cycle); // start with 50% duty cycle

    usleep(5000000); // keep the LED on for 5 seconds

    set_duty_cycle(device, PIN_12_CHANNEL_ID, 0); // turn off the LED

    terminate_pwm(device, PIN_12_CHANNEL_ID);

}

