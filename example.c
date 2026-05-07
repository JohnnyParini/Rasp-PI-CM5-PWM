#include "pwm.h"
#include <stdbool.h>
#include <unistd.h>

/*This code progressively brightens an LED in duty cycle increments of 100000 nanoseconds*/
int main(int argc, char** argv){
    int const clock = 5000000;
    int const period = 1000000;

    device_obj dev_obj = { PIN_12_PATH };
    device_obj *device = &dev_obj;

    initialize_pwm(device, PIN_12_CHANNEL_ID, period);
    

    for (int i = 50000; i < period; i+=100000){
        set_duty_cycle(device, PIN_12_CHANNEL_ID, i);
        usleep(1000000);
    }


    terminate_pwm(device, PIN_12_CHANNEL_ID);

}