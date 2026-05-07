
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

