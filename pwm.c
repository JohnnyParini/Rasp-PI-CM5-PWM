#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

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

bool set_export(device_obj* fs, int pin_channel) {
    char str[INT_TO_STR_SIZE];
    snprintf(str, sizeof(str), "%d", pin_channel);
    return set(fs, "export", str);
}

bool set_enable(device_obj* fs, int pin_channel, bool flag) {
    char key[64];
    snprintf(key, sizeof(key), "pwm%d/enable", pin_channel);
    return set(fs, key, flag ? "1" : "0");
}

bool set_period(device_obj* fs, int pin_channel, int period) {
    char key[64], val[32];
    snprintf(key, sizeof(key), "pwm%d/period", pin_channel);
    snprintf(val, sizeof(val), "%d", period);
    return set(fs, key, val);
}


bool set_duty_cycle(device_obj* fs, int pin_channel, int duty_cycle) {
    char key[64], val[32];
    snprintf(key, sizeof(key), "pwm%d/duty_cycle", pin_channel);
    snprintf(val, sizeof(val), "%d", duty_cycle);
    return set(fs, key, val);
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

// POTENTIALLY UNSTABLE: USE WITH CAUTION, IF AT ALL
// runs bash commands that automatically configure the pwm in config.txt
// clock_cycle_ns defaults to 5000000 ns when given a negative value
// pin number should be 12, 13, 18, or 19
int autoconfig_dtoverlay(int clock_cycle_ns, int pin_number){

    if (clock_cycle_ns < 0){clock_cycle_ns = 5000000;}
    if (pin_number != 12 || pin_number != 13 || pin_number != 18 || pin_number != 19){
        printf("Invalid pin number provided, defaulting to pin 12");
        pin_number = 12;
    }

    char cmd[256]; 
    snprintf(cmd, sizeof(cmd), "echo 'dtoverlay=pwm,pin=%d,func=4,clock=%d' >> /boot/firmware/config.txt", pin_number, clock_cycle_ns);

    int result = 0;
    result = system("sed -i '/dtoverlay=pwm/d' /boot/firmware/config.txt");
    if (result != 0) {
        printf("Failed to remove existing dtoverlay to config.txt\n");
        return -1;
    }
    result = system(cmd);
    if (result != 0) {
        printf("Failed to execute command: %s\n", cmd);
        return -1;
    }

    printf("EXEC REBOOT");

    sync();
    system("reboot");
    return 0;
}



