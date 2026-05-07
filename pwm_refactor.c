#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define INT_TO_STR_SIZE 20
#define CYCLE_COUNT 4
#define PIN_CHANNEL 0
#define PWM_FS_PATH "/sys/class/pwm/pwmchip0"
#define PWM_TEST_PATH "/tmp/pwm_test/pwmchip0"


typedef struct {
    char* pin_path;
} device_obj;

bool set(device_obj* fs, char* info, char* value)
{
    printf(" info: %s  value:%s\n", info, value);
    char thefileName[256];
    snprintf(thefileName, sizeof(thefileName), "%s/%s", fs->pin_path, info);
    FILE* thefile = fopen(thefileName,"w");
    if(!thefile) {
        printf("Failed to open file: %s\n", thefileName);
        return false;
    }
    // no buffer
    setbuf(thefile, NULL);
    int result = fwrite(value, strlen(value),1,thefile);
    printf("fwrite result: %d\n", result);
    if(result != 1) {
        printf("Failed to write to file: %s\n", thefileName);
        fclose(thefile);
        return false;
    }
    fclose(thefile);
    usleep(10000); // add a small delay

    return true;
}

bool set_unexport(device_obj* fs, int pin_channel) {
    char str[INT_TO_STR_SIZE];
    snprintf(str, sizeof(str), "%d", pin_channel);
    return set(fs, "unexport", str);
}

FILE* get_export_file(device_obj* fs) {
    char thefileName[256];
    snprintf(thefileName, sizeof(thefileName), "%s/export", fs->pin_path);
    FILE* thefile = fopen(thefileName,"w");
    if(!thefile) {
        printf("Failed to open file: %s\n", thefileName);
        return NULL;
    }
    // no buffer
    setbuf(thefile, NULL);
    return thefile;
}

bool set_export(device_obj* fs, int pin_channel) {
    char str[INT_TO_STR_SIZE];
    snprintf(str, sizeof(str), "%d", pin_channel);
    return set(fs, "export", str);
}

FILE* get_enable_file(device_obj* fs, int pin_channel) {
    char thefileName[256];
    snprintf(thefileName, sizeof(thefileName), "%s/pwm%d/enable", fs->pin_path, pin_channel);
    FILE* thefile = fopen(thefileName,"w");
    if(!thefile) {
        printf("Failed to open file: %s\n", thefileName);
        return NULL;
    }
    // no buffer
    setbuf(thefile, NULL);
    return thefile;
}

bool set_enable(device_obj* fs, int pin_channel, bool flag) {
    char key[64];
    snprintf(key, sizeof(key), "pwm%d/enable", pin_channel);
    return set(fs, key, flag ? "1" : "0");
}

FILE* get_enable_file(device_obj* fs, int pin_channel) {
    char thefileName[256];
    snprintf(thefileName, sizeof(thefileName), "%s/pwm%d/enable", fs->pin_path, pin_channel);
    FILE* thefile = fopen(thefileName,"w");
    if(!thefile) {
        printf("Failed to open file: %s\n", thefileName);
        return NULL;
    }
    // no buffer
    setbuf(thefile, NULL);
    return thefile;
}

bool set_period(device_obj* fs, int pin_channel, int period) {
    char key[64], val[32];
    snprintf(key, sizeof(key), "pwm%d/period", pin_channel);
    snprintf(val, sizeof(val), "%d", period);
    return set(fs, key, val);
}

FILE* get_period_file(device_obj* fs, int pin_channel) {
    char thefileName[256];
    snprintf(thefileName, sizeof(thefileName), "%s/pwm%d/period", fs->pin_path, pin_channel);
    FILE* thefile = fopen(thefileName,"w");
    if(!thefile) {
        printf("Failed to open file: %s\n", thefileName);
        return NULL;
    }
    // no buffer
    setbuf(thefile, NULL);
    return thefile;
}

bool set_duty_cycle(device_obj* fs, int pin_channel, int duty_cycle) {
    char key[64], val[32];
    snprintf(key, sizeof(key), "pwm%d/duty_cycle", pin_channel);
    snprintf(val, sizeof(val), "%d", duty_cycle);
    return set(fs, key, val);
}

FILE* get_duty_cycle_file(device_obj* fs, int pin_channel) {
    char thefileName[256];
    snprintf(thefileName, sizeof(thefileName), "%s/pwm%d/duty_cycle", fs->pin_path, pin_channel);
    FILE* thefile = fopen(thefileName,"w");
    if(!thefile) {
        printf("Failed to open file: %s\n", thefileName);
        return NULL;
    }
    // no buffer
    setbuf(thefile, NULL);
    return thefile;
}

// kills the pwm chip, no longer writable
int terminate_pwm(device_obj* device, int pin_channel){
    set_enable(device, pin_channel, false);
    set_unexport(device, pin_channel);
    return 0;
}
// initializes the pwm.
// period should be in nanoseconds
int initialize_pwm(device_obj* device, int pin_channel, int period_in_ns){

    // safety net, ensures that the pwm is truly reset before beginning
    // should be unneccessary if terminate is called properly at the end of every program
    terminate_pwm(device, pin_channel);


    set_export(device, pin_channel);
    usleep(1000000);
    set_period(device, pin_channel, period_in_ns);
    set_duty_cycle(device, pin_channel, 0);
    set_enable(device, pin_channel, true);
    return 0;
}


