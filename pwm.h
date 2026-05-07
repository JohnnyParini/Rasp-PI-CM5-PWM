#include <stdbool.h>

#ifndef PWM_H
#define PWM_H

#define PIN_12_PATH "/sys/class/pwm/pwmchip0"
#define PIN_12_CHANNEL_ID 0

typedef struct {
    char* pin_path;
} device_obj;

bool set(device_obj* fs, char* info, char* value);

bool set_unexport(device_obj* fs, int pin_channel);

bool set_export(device_obj* fs, int pin_channel);

bool set_enable(device_obj* fs, int pin_channel, bool flag);

bool set_period(device_obj* fs, int pin_channel, int period);

bool set_duty_cycle(device_obj* fs, int pin_channel, int duty_cycle);

int initialize_pwm(device_obj* device, int pin_channel, int period_in_ns);

int terminate_pwm(device_obj* fs, int pin_channel);

#endif