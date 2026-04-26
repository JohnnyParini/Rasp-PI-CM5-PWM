# Rasp-PI-CM5-PWM
A script to trigger PWM signals on a raspberry pi compute module 5

# Configuration:
In a future update, configuration will be automated by selecting a pin. For now, however, it must be done manually.
In /boot/firmware/config.txt, include this line to activate pin 12: dtoverlay=pwm,pin=12,func=4,clock=5000000
Then, reboot the pi. PWM functionality should now be enabled for GPIO12 on the pi. This can be verified by running the command "sudo pinctrl get 12"
If the pin was reset properly, then the output should read: "12: a0    pd | lo // GPIO12 = PWM0_CHAN0"
PWM0 indicates that the pin is associated with pwmchip0, while CHAN0 indicates that its channel id is 0

# Constants
- PIN_12_PATH:
  Slight misnomer-this is a path to pwmchip0, rather than pin 12, however, it is a necessary component of the path to pin 12. Note that all available pwm pins on the pi should use this chip path, so it need not change between pins
  Equivalent to "/sys/class/pwm/pwmchip0"
- PIN_12_CHANNEL_ID:
  The channel on pwmchip0 that corresponds to pin 12. 
  Equivalent to 0


# Structs

- typedef struct { char* pin_path; } device_obj :
  A struct that wraps a path to the pin that is sending the pwm signal.
  As an example, if one wanted to control pin 12, PIN_12_PATH should be passed in

# Function API

- bool set(device_obj* fs, char* info, char* value):
  The function that writes to files/the pin itself.
  Should not need to be called by the user. Use the appropriate wrapper classes instead.

- bool set_unexport(device_obj* fs, int pin_channel):
  Removes a pin, preventing it from being written to.

- bool set_export(device_obj* fs, int pin_channel):
  Adds a pin, allowing it to be written to.

- bool set_enable(device_obj* fs, int pin_channel, bool flag):
  Physically enables a pin, allowing it to send a signal. When flag is set to true, a pin is enabled; when it is set to false, the pin is disabled.

- bool set_period(device_obj* fs, int pin_channel, int period):
  Sets the pwm wave period. Period should be provided in nanoseconds.
  
- bool set_duty_cycle(device_obj* fs, int pin_channel, int duty_cycle):
  Sets the duty cycle of PWM wave. Duty cycle should be provided in nanoseconds.

 - int initialize_pwm(device_obj* device, int pin_channel, int period_in_ns);
   A wrapper function that initializes the pin, i.e. exports and enables it. It should be called at the beginning of the program

- int terminate_pwm(device_obj* fs, int pin_channel);
  A wrapper function that terminates the pin, i.e. unexports and disables it. It should be called at the end of the program


